/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:46 by teando            #+#    #+#             */
/*   Updated: 2025/02/07 23:56:19 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	one_philo_must_die(t_philo *philo)
{
	print_state(philo, "has taken a fork");
	usleep(philo->info->time_to_die * 1000);
	print_state(philo, "died");
	philo->info->is_finished = 1;
}

/*
** フォークを取る (非対称: 偶数IDは右->左、奇数IDは左->右)
*/
static void	take_forks(t_philo *philo)
{
	t_info	*info;
	int		id_even;

	info = philo->info;
	id_even = (philo->id % 2 == 0);
	if (id_even)
	{
		pthread_mutex_lock(&info->forks[philo->right_fork]);
		print_state(philo, "has taken a fork");
		pthread_mutex_lock(&info->forks[philo->left_fork]);
		print_state(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(&info->forks[philo->left_fork]);
		print_state(philo, "has taken a fork");
		pthread_mutex_lock(&info->forks[philo->right_fork]);
		print_state(philo, "has taken a fork");
	}
}

static void	drop_forks(t_philo *philo)
{
	t_info	*info;

	info = philo->info;
	pthread_mutex_unlock(&info->forks[philo->left_fork]);
	pthread_mutex_unlock(&info->forks[philo->right_fork]);
}

/*
** 哲学者のメインルーチン
** - 考える -> フォークを取る -> 食べる -> フォークを離す -> 眠る -> ループ
*/
void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->info->nb_philo == 1)
	{ /* 哲学者が1人の場合はフォーク1本しかないので、すぐ死ぬケース */
		one_philo_must_die(philo);
		return (NULL);
	}
	while (!philo->info->is_finished)
	{
		print_state(philo, "is thinking");
		take_forks(philo);
		print_state(philo, "is eating"); // 食事
		philo->last_eat_time = get_time_ms();
		philo->eat_count++;
		smart_sleep(philo->info->time_to_eat);
		drop_forks(philo);
		if (philo->info->must_eat_count > 0 // 全員必要回数を食べたか判定（オプション）
			&& philo->eat_count >= philo->info->must_eat_count)
			break ;
		print_state(philo, "is sleeping"); // 眠る
		smart_sleep(philo->info->time_to_sleep);
	}
	return (NULL);
}
