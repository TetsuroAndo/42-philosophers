/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 00:03:42 by teando            #+#    #+#             */
/*   Updated: 2025/04/30 06:12:50 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline int	start_sim(t_data *d)
{
	long		i;
	pthread_t	ovs;

	d->start_ts = now_ms();
	i = -1;
	while (++i < d->n_philo)
	{
		d->philos[i].last_meal = d->start_ts;
		if (pthread_create(&d->philos[i].th, NULL, life, &d->philos[i]) == -1)
			return (set_stop(d), puterr_ret("Failed to create Philosopher thread\n"));
	}
	if (pthread_create(&ovs, NULL, observer, d) == -1)
		return (set_stop(d), puterr_ret("Failed to create Observer thread\n"));
	pthread_join(ovs, NULL);
	while (--i >= 0)
		pthread_join(d->philos[i].th, NULL);
	return (0);
}

int main(int ac, char **av)
{
	t_data	d;

	if (ac < 5 || ac > 6)
		return (usage());
	if (parse_args(&d, ac, av))
		return (puterr_ret("Bad arguments\n"));
	if (init_data(&d))
	{
		return (puterr_ret("Initialize Philosophers failed\n"));
	}
	if (start_sim(&d))
	{
		destroy_data(&d);
		return (puterr_ret("Simulation failed\n"));
	}
	destroy_data(&d);
	return (EXIT_SUCCESS);
}
