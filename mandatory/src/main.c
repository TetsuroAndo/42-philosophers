/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 18:35:15 by teando            #+#    #+#             */
/*   Updated: 2025/02/07 20:31:05 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline void	usage(void)
{
	printf("Usage: ./philo <num_of_philos> <time_to_die> <time_to_eat> "
			"<time_to_sleep> [num_of_must_eat]\n\n");
	printf("num_of_philos       number of philosophers and nunber of forks\n");
	printf("time_to_die [ms]    the time in when they must start eating\n");
	printf("time_to_eat [ms]    the time it takes them to eat\n");
	printf("time_to_sleep [ms]  the amount of sleep they need\n");
	printf("num_of_must_eat     number of times each philosopher must eat\n");
}

static int	args_check(int ac, char **av)
{
	(void)av;
	if (ac < 5 || ac > 6)
	{
		usage();
		return (1);
	}
	return (0);
}

int	main(int ac, char **av)
{
	t_data	data;

	if (args_check(ac, av))
	{
		usage();
		return (1);
	}
	if (init_data(&data, ac, av) == 1)
	{
		usage();
		return (clean_up(&data), 1);
	}
	if (init_philos(&data) == 1)
		return (clean_up(&data), 1);
	if (start_simulation(&data) == 1)
		return (clean_up(&data), 1);
	return (clean_up(&data), 0);
}
