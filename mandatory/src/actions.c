/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:46 by teando            #+#    #+#             */
/*   Updated: 2024/12/11 19:03:47 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	philo_eat(t_philo *philo)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(philo->left_fork);
	print_action(data, philo->id, "has taken a fork");
	pthread_mutex_lock(philo->right_fork);
	print_action(data, philo->id, "has taken a fork");
	print_action(data, philo->id, "is eating");
	pthread_mutex_lock(&data->meal_check);
	philo->last_meal = get_time();
	philo->eaten_meals++;
	pthread_mutex_unlock(&data->meal_check);
	smart_sleep(data->time_to_eat);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void	philo_sleep_think(t_philo *philo)
{
	t_data	*data;

	data = philo->data;
	print_action(data, philo->id, "is sleeping");
	smart_sleep(data->time_to_sleep);
	print_action(data, philo->id, "is thinking");
}
