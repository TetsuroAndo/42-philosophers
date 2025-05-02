/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   err.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 20:48:30 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 08:55:14 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	puterr(const char *s)
{
	if (write(STDERR_FILENO, s, ft_strlen(s)) == -1)
		exit(EXIT_FAILURE);
	return (0);
}

void	usage(char **av)
{
	puterr("Usage: ");
	if (av && av[0])
		puterr(av[0]);
	else
		puterr("./philo");
	puterr(" <num_philos> <time_die> <time_eat> <time_sleep> [num_must_eat]\n\n");
	puterr("num_philos       number of philosophers and nunber of forks\n");
	puterr("time_die [ms]    the time in when they must start eating\n");
	puterr("time_eat [ms]    the time it takes them to eat\n");
	puterr("time_sleep [ms]  the amount of sleep they need\n");
	puterr("num_must_eat     number of times each philosopher must eat\n");
	exit(EXIT_FAILURE);
}

int	puterr_ret(char *msg)
{
	puterr("Error: ");
	puterr(msg);
	puterr("\n");
	return (EXIT_FAILURE);
}

void	puterr_exit(char *msg)
{
	puterr("Error: ");
	puterr(msg);
	puterr("\n");
	exit(EXIT_FAILURE);
}
