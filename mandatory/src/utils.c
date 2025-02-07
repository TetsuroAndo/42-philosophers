/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:57 by teando            #+#    #+#             */
/*   Updated: 2025/02/07 23:01:27 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** 現在時刻(ミリ秒)を取得
*/
long	get_time_ms(void)
{
	struct timeval	tv;
	long			ms;

	gettimeofday(&tv, NULL);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}

/*
** usleepのラッパ: ミリ秒単位で分かりやすく呼び出す
*/
void	smart_sleep(long time_in_ms)
{
	long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < time_in_ms)
		usleep(100);
}

/*
** 哲学者の状態を出力（mutexで保護）
*/
void	print_state(t_philo *philo, char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&philo->info->print_lock);
	if (!philo->info->is_finished)
	{
		timestamp = get_time_ms() - philo->info->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, msg);
	}
	pthread_mutex_unlock(&philo->info->print_lock);
}
