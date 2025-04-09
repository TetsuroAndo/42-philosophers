/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 18:35:15 by teando            #+#    #+#             */
/*   Updated: 2025/03/23 21:15:10 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int main(int ac, char **av)
{
	t_info info;
	t_philo *philos;
	int i;

	if (ac < 5 || ac > 6)
		return (usage());
	if (parse_args(&info, ac, av) != 0)
		return (usage());
	if (init_philos(&philos, &info) != 0)
		return (free(info.forks), print_error("init_philos failed"));
	start_simulation(philos, &info);
	i = -1;
	while (++i < info.nb_philo)
		pthread_mutex_destroy(&info.forks[i]);
	pthread_mutex_destroy(&info.print_lock);
	free(info.forks);
	free(philos);
	return (EXIT_SUCCESS);
}
