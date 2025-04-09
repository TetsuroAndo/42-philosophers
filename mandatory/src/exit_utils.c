/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   puts.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 20:48:30 by teando            #+#    #+#             */
/*   Updated: 2025/03/23 20:51:38 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int usage(void)
{
    printf("Usage: ./philo <num_of_philos> <time_to_die> <time_to_eat> "
           "<time_to_sleep> [num_of_must_eat]\n\n");
    printf("num_of_philos       number of philosophers and nunber of forks\n");
    printf("time_to_die [ms]    the time in when they must start eating\n");
    printf("time_to_eat [ms]    the time it takes them to eat\n");
    printf("time_to_sleep [ms]  the amount of sleep they need\n");
    printf("num_of_must_eat     number of times each philosopher must eat\n");
    return (EXIT_FAILURE);
}

int print_error(char *msg)
{
    printf("Error: %s\n", msg);
    return (EXIT_FAILURE);
}
