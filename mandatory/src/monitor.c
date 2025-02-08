/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:28 by teando            #+#    #+#             */
/*   Updated: 2025/02/08 00:40:05 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** 全員が必要回数を食べ終わったかチェック
*/
static int	all_philos_ate_enough(t_philo *philos)
{
	t_info	*info;
	int		i;

	info = philos[0].info;
	if (info->must_eat_count <= 0)
		return (0);
	i = 0;
	while (i < info->nb_philo)
	{
		if (philos[i].eat_count < info->must_eat_count)
			return (0);
		i++;
	}
	return (1);
}

/*
** モニタースレッド
** - 各哲学者がtime_to_dieを超えたか
** - 全員が指定回数食べたか
** のチェックを繰り返す
*/
void	*monitor(void *arg)
{
	t_philo	*philos;
	t_info	*info;
	int		i;

	philos = (t_philo *)arg;
	info = philos[0].info;
	while (!info->is_finished)
	{
		i = 0;
		while (i < info->nb_philo && !info->is_finished)
		{
			// 最後に食べてからtime_to_dieを超えたら死亡
			if ((get_time_ms() - philos[i].last_eat_time) > info->time_to_die)
			{
				print_state(&philos[i], "died");
				info->is_finished = 1;
			}
			i++;
		}
		// 全員が十分食べたか
		if (!info->is_finished && all_philos_ate_enough(philos))
			info->is_finished = 1;
		usleep(1000);
	}
	return (NULL);
}
