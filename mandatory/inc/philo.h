/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 22:53:35 by teando            #+#    #+#             */
/*   Updated: 2025/02/07 23:47:31 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

/*
** データ構造定義
*/

typedef struct s_info
{
	int nb_philo;       // 哲学者の数
	long time_to_die;   // ミリ秒
	long time_to_eat;   // ミリ秒
	long time_to_sleep; // ミリ秒
	int must_eat_count; // 各哲学者が食べるべき回数（オプション）
	int is_finished;    // シミュレーション終了フラグ

	pthread_mutex_t *forks;     // フォーク(ミューテックス)を格納する配列
	pthread_mutex_t print_lock; // ログ出力制御用のミューテックス

	long start_time; // シミュレーション開始時刻（ミリ秒）
}		t_info;

typedef struct s_philo
{
	int id;              // 哲学者ID (1~n)
	int eat_count;       // 食事回数
	long last_eat_time;  // 最後に食べた時刻
	int left_fork;       // 左フォークのインデックス
	int right_fork;      // 右フォークのインデックス
	t_info *info;        // 共通情報へのポインタ
	pthread_t thread_id; // スレッドID
}		t_philo;

/* 関数プロトタイプ宣言 */

/* main.c */
int		print_error(char *msg);

/* init.c */
int		init_info(t_info *info, int argc, char **argv);
int		init_philos(t_philo **philos, t_info *info);
void	start_simulation(t_philo *philos, t_info *info);

/* routine.c */
void	*routine(void *arg);

/* monitor.c */
void	*monitor(void *arg);

/* utils.c */
long	get_time_ms(void);
void	smart_sleep(long time_in_ms);
void	print_state(t_philo *philo, char *msg);

#endif
