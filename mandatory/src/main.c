/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 18:35:15 by teando            #+#    #+#             */
/*   Updated: 2024/12/11 19:12:42 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	args_check(int ac, char **av)
{
	(void)av;
	if (ac < 5 || ac > 6)
	{
		printf("Usage: %s ", av[0]);
		printf("num_philos time_to_die time_to_eat time_to_sleep ");
		printf("[must_eat_count]\n");
		return (1);
	}
	return (0);
}

int	main(int ac, char **av)
{
	t_data	data;

	if (args_check(ac, av))
		return (1);
	if (init_data(&data, ac, av) == 1)
		return (1);
	if (init_philos(&data) == 1)
		return (clean_up(&data), 1);
	if (start_simulation(&data) == 1)
		return (clean_up(&data), 1);
	return (clean_up(&data), 0);
}
