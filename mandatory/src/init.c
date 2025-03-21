/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 18:35:21 by teando            #+#    #+#             */
/*   Updated: 2025/03/21 22:29:39 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <limits.h>

static long ft_atol(const char *nptr)
{
	unsigned long cutoff;
	int neg;
	int tmp;
	unsigned long res;

	neg = 0;
	res = 0;
	while (*nptr == ' ' || (*nptr >= 9 && *nptr <= 13))
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		neg = *nptr++ == '-';
	cutoff = (unsigned long)LONG_MAX ^ -neg;
	while (*nptr >= '0' && *nptr <= '9')
	{
		res *= 10;
		tmp = *nptr++ - '0';
		if (res > cutoff || cutoff - res < (unsigned long)tmp)
			return (cutoff);
		res += tmp;
	}
	if (neg)
		return (-res);
	return (res);
}

/*
** 引数をパースしてinfoを初期化
**
	- number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
*/
int init_info(t_info *info, int argc, char **argv)
{
	int i;

	memset(info, 0, sizeof(t_info));
	info->nb_philo = (int)ft_atol(argv[1]);
	info->time_to_die = ft_atol(argv[2]);
	info->time_to_eat = ft_atol(argv[3]);
	info->time_to_sleep = ft_atol(argv[4]);
	info->must_eat_count = -1;
	if (argc == 6)
		info->must_eat_count = (int)ft_atol(argv[5]);
	if (info->nb_philo <= 0 || info->time_to_die <= 0 || info->time_to_eat <= 0 || info->time_to_sleep <= 0)
		return (1);
	info->is_finished = 0;
	info->forks = malloc(sizeof(pthread_mutex_t) * info->nb_philo);
	if (!info->forks)
		return (1);
	i = -1;
	while (++i < info->nb_philo)
		if (pthread_mutex_init(&info->forks[i], NULL) != 0)
			return (1);
	if (pthread_mutex_init(&info->print_lock, NULL) != 0)
		return (1);
	info->start_time = get_time_ms();
	return (0);
}

/*
** 哲学者配列を確保して初期化
*/
int init_philos(t_philo **philos, t_info *info)
{
	int i;

	*philos = malloc(sizeof(t_philo) * info->nb_philo);
	if (!*philos)
		return (1);
	i = -1;
	while (++i < info->nb_philo)
	{
		(*philos)[i].id = i + 1;
		(*philos)[i].eat_count = 0;
		(*philos)[i].last_eat_time = info->start_time;
		(*philos)[i].info = info;
		/*
		** 左右のフォークを設定
		** デッドロック防止のために小さいインデックスを先に取るようにする
		*/
		if (i == (info->nb_philo - 1))
		{
			// 最後の哲学者は可能性の高い肯実なデッドロックに対応
			(*philos)[i].left_fork = 0;
			(*philos)[i].right_fork = i;
		}
		else
		{
			(*philos)[i].left_fork = i;
			(*philos)[i].right_fork = (i + 1) % info->nb_philo;
		}
	}
	return (0);
}

/*
** スレッドを生成 & 監視スレッドを起動し、シミュレーションを開始
*/
void start_simulation(t_philo *philos, t_info *info)
{
	pthread_t mon_thread;
	int i;

	/* シミュレーション開始時間を再設定 */
	info->start_time = get_time_ms();
	
	/* 哲学者の最後の食事時間を正確に設定 */
	i = -1;
	while (++i < info->nb_philo)
		philos[i].last_eat_time = info->start_time;
	
	/* 哲学者スレッド作成 */
	i = -1;
	while (++i < info->nb_philo)
		pthread_create(&philos[i].thread_id, NULL, routine, &philos[i]);

	/* 少し待機してから監視スレッドを開始 */
	usleep(100);
	
	/* 監視スレッド作成（哲学者の生存状態をチェック） */
	pthread_create(&mon_thread, NULL, monitor, (void *)philos);

	/* スレッドの終了待ち */
	i = -1;
	while (++i < info->nb_philo)
		pthread_join(philos[i].thread_id, NULL);
	
	/* 監視スレッドの安全な終了処理 */
	// 監視スレッドが確実に終了するよう少し待機
	usleep(10000);
	pthread_detach(mon_thread);
}
