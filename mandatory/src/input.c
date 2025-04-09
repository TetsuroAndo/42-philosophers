/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/23 21:10:11 by teando            #+#    #+#             */
/*   Updated: 2025/03/23 21:14:59 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <limits.h>

static long ft_atol(const char *nptr)
{
    unsigned long cutoff;
    int neg;
    int tmp;
    unsigned long res;

    neg = 0;
    res = 0;
    while (*nptr == ' ' || (*nptr >= 9 && *nptr <= 13))
        nptr++;
    if (*nptr == '+' || *nptr == '-')
        neg = *nptr++ == '-';
    cutoff = (unsigned long)LONG_MAX ^ -neg;
    while (*nptr >= '0' && *nptr <= '9')
    {
        res *= 10;
        tmp = *nptr++ - '0';
        if (res > cutoff || cutoff - res < (unsigned long)tmp)
            return (cutoff);
        res += tmp;
    }
    if (neg)
        return (-res);
    return (res);
}

/*
** 引数をパースしてinfoを初期化
**
    - number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
*/
int parse_args(t_info *info, int argc, char **argv)
{
    int i;

    memset(info, 0, sizeof(t_info));
    info->nb_philo = (int)ft_atol(argv[1]);
    info->time_to_die = ft_atol(argv[2]);
    info->time_to_eat = ft_atol(argv[3]);
    info->time_to_sleep = ft_atol(argv[4]);
    info->must_eat_count = -1;
    if (argc == 6)
        info->must_eat_count = (int)ft_atol(argv[5]);
    if (info->nb_philo <= 0 || info->time_to_die <= 0 || info->time_to_eat <= 0 || info->time_to_sleep <= 0)
        return (1);
    info->is_finished = 0;
    info->forks = malloc(sizeof(pthread_mutex_t) * info->nb_philo);
    if (!info->forks)
        return (1);
    i = -1;
    while (++i < info->nb_philo)
        if (pthread_mutex_init(&info->forks[i], NULL) != 0)
            return (1);
    if (pthread_mutex_init(&info->print_lock, NULL) != 0)
        return (1);
    info->start_time = get_time_ms();
    return (0);
}
