/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:46 by teando            #+#    #+#             */
/*   Updated: 2025/02/08 22:15:43 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** 哲学者が一人しかいない場合用の処理
** → フォークが1本しかなく、time_to_die後に確実に死亡する
*/
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

/*
** フォークを置く
*/
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
	{
		one_philo_must_die(philo);
		return (NULL);
	}
	/*
	** 衝突を緩和するため、
	** 偶数IDの哲学者は開始直後に少しだけ待機
	*/
	if (philo->id % 2 == 0)
		usleep(1000); // 1ミリ秒待機に増加
	while (!philo->info->is_finished)
	{
		/* 考える */
		print_state(philo, "is thinking");
		/*
		** もし「考えている時間」が必要なら以下のようにsleep可
		** smart_sleep(10); // 例えば10msなど
		*/
		/* フォークを取る → 食べる */
		take_forks(philo);
		
		// 食事状態の更新をアトミックに行う
		pthread_mutex_lock(&philo->info->print_lock);
		philo->last_eat_time = get_time_ms();
		philo->eat_count++;
		print_state(philo, "is eating");
		pthread_mutex_unlock(&philo->info->print_lock);
		
		smart_sleep(philo->info->time_to_eat);
		drop_forks(philo);
		/* 規定回数食べ終わったかチェック (オプション) */
		if (philo->info->must_eat_count > 0
			&& philo->eat_count >= philo->info->must_eat_count)
			break ;
		/* 眠る */
		print_state(philo, "is sleeping");
		smart_sleep(philo->info->time_to_sleep);
	}
	return (NULL);
}
