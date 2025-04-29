/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   err.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 20:48:30 by teando            #+#    #+#             */
/*   Updated: 2025/04/30 05:16:46 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int puterr(const char *s)
{
	if (write (STDERR_FILENO, s, ft_strlen(s)) == -1)
		return (-1);
	return (0);
}

int usage(void)
{
    puterr("Usage: ./philo <num_philos> <time_die> <time_eat> "
           "<time_sleep> [num_must_eat]\n\n");
    puterr("num_philos       number of philosophers and nunber of forks\n");
    puterr("time_die [ms]    the time in when they must start eating\n");
    puterr("time_eat [ms]    the time it takes them to eat\n");
    puterr("time_sleep [ms]  the amount of sleep they need\n");
    puterr("num_must_eat     number of times each philosopher must eat\n");
    return (EXIT_FAILURE);
}

int puterr_ret(char *msg)
{
    puterr("Error: ");
    puterr(msg);
    puterr("\n");
    return (EXIT_FAILURE);
}


