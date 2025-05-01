/* ************************************************************************** */
/*  philo_bonus_demo.c – 極小サンプル (プロセス + セマフォ)                */
/* ************************************************************************** */
#define  _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>      /* O_CREAT, O_EXCL */
#include <semaphore.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

typedef struct s_cfg
{
	int  n_philo;
	int  t_die;
	int  t_eat;
	int  t_sleep;
	int  must_eat;   /* -1 なら無限 */
}	t_cfg;

/*―――――――――― 時間ヘルパ ――――――――――*/
static long	now_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000);
}
static void	msleep_exact(int ms)
{
	long start = now_ms();
	while (now_ms() - start < ms)
		usleep(100);		/* 100 µs ポーリングで誤差最小化 */
}

/*―――――――――― 子プロセス (哲学者) 本体 ――――――――――*/
static void	philo_routine(int id, sem_t *forks, const t_cfg *cfg, long t0)
{
	int	ate = 0;

	while (cfg->must_eat < 0 || ate < cfg->must_eat)
	{
		printf("%ld %d is thinking\n", now_ms() - t0, id);
		/* ↓ Dijkstra 式：フォーク 2 本まとめて確保 (デッドロック回避) */
		sem_wait(forks);  /* 1 本目 */
		sem_wait(forks);  /* 2 本目 */
		printf("%ld %d has taken a fork\n", now_ms() - t0, id);
		printf("%ld %d has taken a fork\n", now_ms() - t0, id);
		printf("%ld %d is eating\n",        now_ms() - t0, id);
		msleep_exact(cfg->t_eat);
		sem_post(forks);
		sem_post(forks);
		ate++;
		printf("%ld %d is sleeping\n",      now_ms() - t0, id);
		msleep_exact(cfg->t_sleep);
	}
	exit(EXIT_SUCCESS);
}

/*―――――――――― 親プロセス (司会) ――――――――――*/
int	main(int ac, char **av)
{
	if (ac < 5 || ac > 6)
	{
		fprintf(stderr, "Usage: %s n t_die t_eat t_sleep [must_eat]\n", av[0]);
		return (EXIT_FAILURE);
	}
	t_cfg cfg = {
		.n_philo  = atoi(av[1]),
		.t_die    = atoi(av[2]),
		.t_eat    = atoi(av[3]),
		.t_sleep  = atoi(av[4]),
		.must_eat = (ac == 6) ? atoi(av[5]) : -1
	};
	if (cfg.n_philo <= 0)
		return (fprintf(stderr, "n_philo must be >0\n"), EXIT_FAILURE);

	/* ― セマフォ生成 ―
	   名前付きなので fork 後に再オープンしても共有できる */
	const char *sem_name = "/forks_sem_example";
	sem_unlink(sem_name);                     /* 前回残骸を掃除 */
	sem_t *forks = sem_open(sem_name, O_CREAT | O_EXCL, 0600, cfg.n_philo);
	if (forks == SEM_FAILED)
		return (perror("sem_open"), EXIT_FAILURE);

	long t0 = now_ms();
	pid_t *pids = calloc(cfg.n_philo, sizeof(pid_t));
	for (int i = 0; i < cfg.n_philo; ++i)
	{
		if ((pids[i] = fork()) == 0)
		{	/* 子プロセス */
			sem_t *child_forks = sem_open(sem_name, 0);
			if (child_forks == SEM_FAILED)
				exit(perror("child sem_open"), EXIT_FAILURE);
			philo_routine(i + 1, child_forks, &cfg, t0);
		}
		else if (pids[i] < 0)
			return (perror("fork"), EXIT_FAILURE);
	}

	/* ――――― 親：子の終了待ち → 後片付け ――――― */
	for (int i = 0; i < cfg.n_philo; ++i)
		waitpid(pids[i], NULL, 0);
	sem_close(forks);
	sem_unlink(sem_name);
	free(pids);
	return (EXIT_SUCCESS);
}
