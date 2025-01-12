/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 18:35:29 by teando            #+#    #+#             */
/*   Updated: 2025/01/12 19:56:28 by teando           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "philo.h"

static void	*philo_routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;

	philo = (t_philo *)arg;
	data = philo->data;
	philo->last_meal = get_time();
	while (1)
	{
		philo_eat(philo);
		if (data->died || data->all_ate)
			break ;
		philo_sleep_think(philo);
		if (data->died || data->all_ate)
			break ;
	}
	return (NULL);
}

int	start_simulation(t_data *data)
{
	pthread_t	monitor_tid;
	int			i;

	i = -1;
	data->start_time = get_time();
	while (++i < data->num_philos) // スレッド作成
	{
		if (pthread_create(&data->philos[i].thread_id, NULL, philo_routine,
				&data->philos[i]) != 0)
			return (1);
	}
	if (pthread_create(&monitor_tid, NULL, monitor_death, data) != 0) // 死亡監視スレッド
		return (1);
	i = -1;
	while (++i < data->num_philos) // スレッド結合
		pthread_join(data->philos[i].thread_id, NULL);
	pthread_join(monitor_tid, NULL);
	return (0);
}
