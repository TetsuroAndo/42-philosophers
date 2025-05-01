/* ************************************************************************** */
/*  philo_bonus_perm.c – mmap なし・許可関数のみ版                            */
/* ************************************************************************** */
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

/*───── ご指定の構造体そのまま ─────*/
typedef struct s_philo_shm      /* ← 本実装では“各プロセス内のローカル構造体” */
{
	long last_meal_ms;
	long eat_count;
}	t_philo_shm;

typedef struct s_shared
{
	long start_ts;              /* 全プロセス共通で読み出すだけなのでコピーで可 */
	int  stop;                  /* 親しか見ない → プロセス間共有不要       */
	t_philo_shm *ph;            /* 今回は NULL のまま */
}	t_shared;

typedef struct s_cfg
{
	long n_philo;
	long t_die;
	long t_eat;
	long t_sleep;
	long must_eat;              /* -1 = 無限 */
}	t_cfg;

typedef struct s_ctx
{
	long  id;                   /* 1..n */
	t_cfg *cfg;
	t_shared *shm;              /* start_ts 用 */
	sem_t *forks_sem;           /* 在庫 n_philo */
	sem_t *print_sem;           /* binary */
}	t_ctx;

/*───── 時間ユーティリティ ─────*/
static long now_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
}
static void msleep_exact(long ms)
{
	long start = now_ms();
	while (now_ms() - start < ms)
		usleep(200);
}

/*───── ログ出力 (排他付き) ─────*/
static void log_state(t_ctx *c, const char *msg)
{
	sem_wait(c->print_sem);
	printf("%ld %ld %s\n",
	       now_ms() - c->shm->start_ts, c->id, msg);
	sem_post(c->print_sem);
}

/*───── 監視スレッド：自分自身の餓死判定 ─────*/
typedef struct s_mon_arg 
{ 
	t_ctx *c;
	t_philo_shm *self; 
} t_mon_arg;

static void *monitor_fn(void *varg)
{
	t_mon_arg *m = (t_mon_arg *)varg;
	t_ctx *c     = m->c;
	while (1)
	{
		long since = now_ms() - m->self->last_meal_ms;
		if (since >= c->cfg->t_die)
		{
			log_state(c, "died");
			exit(1); /* 非 0 終了で親に異常を知らせる */
		}
		usleep(1000);
	}
	return (NULL);
}

/*───── 子プロセス：行動ループ ─────*/
static void philosopher(t_ctx *c)
{
	t_philo_shm ph = { 
		.last_meal_ms = now_ms(), 
		.eat_count = 0 
	};
	pthread_t   mon_th;
	t_mon_arg   m = { 
		.c = c, 
		.self = &ph 
	};

	if (pthread_create(&mon_th, NULL, monitor_fn, &m) != 0)
	{
		perror("pthread_create");
		exit(1);
	}
	pthread_detach(mon_th); /* join 不要 */

	while (c->cfg->must_eat < 0 || ph.eat_count < c->cfg->must_eat)
	{
		log_state(c, "is thinking");

		sem_wait(c->forks_sem);
		sem_wait(c->forks_sem);
		log_state(c, "has taken a fork");
		log_state(c, "has taken a fork");

		log_state(c, "is eating");
		ph.last_meal_ms = now_ms();
		ph.eat_count++;
		msleep_exact(c->cfg->t_eat);

		sem_post(c->forks_sem);
		sem_post(c->forks_sem);

		log_state(c, "is sleeping");
		msleep_exact(c->cfg->t_sleep);
	}
	exit(0);                    /* 規定回数を食べ終えた → 正常終了 */
}

/*───── 引数パース ─────*/
static int parse_cfg(int ac, char **av, t_cfg *cfg)
{
	if (ac < 5 || ac > 6)
		return (fprintf(stderr,
			"Usage: %s n t_die t_eat t_sleep [must_eat]\n", av[0]), -1);
	cfg->n_philo  = atol(av[1]);
	cfg->t_die    = atol(av[2]);
	cfg->t_eat    = atol(av[3]);
	cfg->t_sleep  = atol(av[4]);
	cfg->must_eat = (ac == 6) ? atol(av[5]) : -1;
	if (cfg->n_philo <= 0)
		return (fprintf(stderr, "n_philo must be >0\n"), -1);
	return 0;
}

/*───── メイン ─────*/
int main(int ac, char **av)
{
	t_cfg cfg;
	if (parse_cfg(ac, av, &cfg))
		return (EXIT_FAILURE);

	/*--- “共有”データは malloc コピーで十分 ---*/
	t_shared shm = { 
		.start_ts = now_ms(), 
		.stop = 0, 
		.ph = NULL 
	};

	/*--- セマフォ ---*/
	const char *forks_name = "/forks_sem_perm";
	const char *print_name = "/print_sem_perm";
	sem_unlink(forks_name);
	sem_unlink(print_name);

	sem_t *forks = sem_open(forks_name, O_CREAT|O_EXCL, 0600, cfg.n_philo);
	sem_t *print = sem_open(print_name, O_CREAT|O_EXCL, 0600, 1);
	if (forks == SEM_FAILED || print == SEM_FAILED)
		return (perror("sem_open"), EXIT_FAILURE);

	pid_t *pids = malloc(sizeof(pid_t) * cfg.n_philo);
	if (!pids)
		return (perror("malloc"), EXIT_FAILURE);

	/*--- 子プロセス生成 ---*/
	for (long i = 0; i < cfg.n_philo; ++i)
	{
				pids[i] = fork();
				if (pids[i] == 0) /* ---- 子 ---- */
				{
					t_ctx c = { .id = i + 1,
								.cfg = &cfg,
								.shm = &shm,
								.forks_sem = sem_open(forks_name, 0),
								.print_sem = sem_open(print_name, 0) };
					if (c.forks_sem == SEM_FAILED || c.print_sem == SEM_FAILED)
					{
						perror("child sem_open");
						exit(1);
					}
					philosopher(&c); /* 戻らない */
				}
				else if (pids[i] < 0)
					return (perror("fork"), EXIT_FAILURE);
	}

	/* 親：誰か死ぬか全員終了するまで待つ */
	int   status;
	long  finished = 0;
	while (finished < cfg.n_philo)
	{
		pid_t pid = waitpid(-1, &status, 0);
		if (pid < 0)
			break;
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
			; /* 正常終了（満腹） */
		else
		{ /* 異常終了 → 誰か死んだ */
			for (long k = 0; k < cfg.n_philo; ++k)
				kill(pids[k], SIGTERM);
		}
		++finished;
	}

	sem_close(forks);
	sem_close(print);
	sem_unlink(forks_name);
	sem_unlink(print_name);
	free(pids);
	return (EXIT_SUCCESS);
}
