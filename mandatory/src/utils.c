/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:57 by teando            #+#    #+#             */
/*   Updated: 2025/02/08 00:02:31 by teando           ###   ########.fr       */
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
	long	elapsed;

	start = get_time_ms();
	while (1)
	{
		elapsed = get_time_ms() - start;
		if (elapsed >= time_in_ms)
			break;
		// 残り時間が10ms以上ならより長く眠る、そうでなければ短く眠る
		if (time_in_ms - elapsed > 10)
			usleep(1000);
		else
			usleep(50);
	}
}

/*
** 哲学者の状態を出力（mutexで保護）
*/
void	print_state(t_philo *philo, char *msg)
{
	long	timestamp;
	static int	print_lock_held = 0;

	// 既にロックを持っている場合はロックしない
	if (!print_lock_held)
		pthread_mutex_lock(&philo->info->print_lock);
	
	if (!philo->info->is_finished)
	{
		timestamp = get_time_ms() - philo->info->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, msg);
	}
	
	if (!print_lock_held)
		pthread_mutex_unlock(&philo->info->print_lock);
}
