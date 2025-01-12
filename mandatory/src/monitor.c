/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:28 by teando            #+#    #+#             */
/*   Updated: 2024/12/11 19:19:56 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	check_death(t_data *data)
{
	int	done_eat;
	int	i;
	int	j;

	i = -1;
	while (++i < data->num_philos && !data->all_ate)
	{
		pthread_mutex_lock(&data->meal_check);
		if ((get_time() - data->philos[i].last_meal) > data->time_to_die)
		{
			print_action(data, data->philos[i].id, "died");
			data->died = 1;
			pthread_mutex_unlock(&data->meal_check);
			return ;
		}
		if (data->must_eat_count > 0)
		{
			done_eat = 0;
			j = -1;
			while (++j < data->num_philos)
			{
				if (data->philos[j].eaten_meals >= data->must_eat_count)
					done_eat++;
			}
			if (done_eat == data->num_philos)
				data->all_ate = 1;
		}
		pthread_mutex_unlock(&data->meal_check);
	}
}

void	*monitor_death(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (!data->died && !data->all_ate)
	{
		check_death(data);
		usleep(1000);
	}
	return (NULL);
}
