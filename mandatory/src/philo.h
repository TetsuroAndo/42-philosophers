/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 22:53:35 by teando            #+#    #+#             */
/*   Updated: 2025/04/30 07:29:31 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <stddef.h>
# include <limits.h>

typedef struct	s_data	t_data;

typedef struct	s_philo
{
	int				id;			/* 各哲学者の固有識別子 */
	long			last_meal;	/* 哲学者が最後に食事をした時刻（ms） */
	int				eat_count;	/* 哲学者が食事をした回数 */
	pthread_mutex_t	meal_mtx;	/* last_mealとeat_countへのアクセスを保護するミューテックス */
	pthread_mutex_t	*fork_l;	/* 左側のフォークのミューテックス */
	pthread_mutex_t	*fork_r;	/* 右側のフォークのミューテックス */
	pthread_t		th;			/* この哲学者のスレッドハンドル */
	t_data			*data;		/* 共有シミュレーションデータへのポインタ */
}					t_philo;

typedef struct	s_data
{
	long			start_ts;	/* シミュレーション開始時刻（ms） */
	long			n_philo;	/* 哲学者の数 */
	long			t_die;		/* 哲学者が食事をしないと死亡するまでの時間（ms） */
	long			t_eat;		/* 哲学者が食事をするのにかかる時間（ms） */
	long			t_sleep;	/* 哲学者が睡眠に費やす時間（ms） */
	long			must_eat;	/* 各哲学者が食べなければならない回数（option） */
	int				stop;		/* シミュレーションを停止すべきかを示すフラグ（1=stop、0=continue） */
	pthread_mutex_t stop_mtx;	/* stopフラグへのアクセスを保護するミューテックス */
	pthread_mutex_t	print_mtx;	/* ステータスメッセージの原子的な出力を確保するミューテックス */
	pthread_mutex_t	*forks;		/* フォークを表すミューテックスの配列 */
	t_philo			*philos;	/* 哲学者構造体の配列 */
}					t_data;

/* main proc */
void				*life(void *arg);
void				*observer(void *arg);

/* data.c */
int		parse_args(t_data *d, int ac, char **av);
int		init_data(t_data *d);
void	destroy_data(t_data *d);
void	destory_philos(t_philo *philos, long n_philos);

/* misc.c */
long	now_ms(void);
void	msleep(long ms, t_data *d);
void	put_state(t_philo *p, char *msg);
void	set_stop(t_data *d);
int		check_stop(t_data *d);

/* puterr.c */
size_t	ft_strlen(const char *s);
int 	puterr(const char *s);
int 	usage(void);
int 	puterr_ret(char *msg);

#endif