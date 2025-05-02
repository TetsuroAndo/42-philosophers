/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 00:03:42 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 11:00:32 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline void	set_start_ts(t_data *d)
{
	long	i;

	pthread_mutex_lock(&d->stop_mtx);
	d->start_ts = now_ms() + 100;
	pthread_mutex_unlock(&d->stop_mtx);
	i = -1;
	while (++i < d->n_philo)
	{
		pthread_mutex_lock(&d->philos[i].meal_mtx);
		d->philos[i].last_meal = d->start_ts;
		pthread_mutex_unlock(&d->philos[i].meal_mtx);
	}
}

static inline long	start_sim(t_data *d)
{
	long		i;
	pthread_t	obs;

	d->start_ts = 0;
	i = -1;
	while (++i < d->n_philo)
	{
		if (pthread_create(&d->philos[i].th, NULL, life, &d->philos[i]))
			return (puterr_ret("Failed to create Philosopher thread"));
	}
	if (pthread_create(&obs, NULL, observer, d))
		return (puterr_ret("Failed to create Observer thread"));
	set_start_ts(d);
	pthread_join(obs, NULL);
	while (--i >= 0)
		pthread_join(d->philos[i].th, NULL);
	return (0);
}

int	main(int ac, char **av)
{
	t_data	d;

	if (ac < 5 || ac > 6)
		return (usage());
	if (parse_args(&d, ac, av))
		return (puterr_ret("Bad arguments"));
	if (init_data(&d))
	{
		return (puterr_ret("Initialize Philosophers failed"));
	}
	if (start_sim(&d))
	{
		destroy_data(&d);
		return (puterr_ret("Simulation failed"));
	}
	destroy_data(&d);
	return (EXIT_SUCCESS);
}
