/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:43:26 by teando            #+#    #+#             */
/*   Updated: 2025/04/30 06:06:07 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

int	parse_args(t_data *d, int ac, char **av)
{
	d->n_philo = ft_atol(av[1]);
	d->t_die = ft_atol(av[2]);
	d->t_eat = ft_atol(av[3]);
	d->t_sleep = ft_atol(av[4]);
	d->must_eat = -1;
	if (ac == 6)
		d->must_eat = ft_atol(av[5]);
	if (d->n_philo <= 0 || d->t_die <= 0 || d->t_eat <= 0 || d->t_sleep <= 0)
		return (1);
	if (d->n_philo > INT_MAX || d->t_die > INT_MAX || d->t_eat > INT_MAX 
		|| d->t_sleep > INT_MAX || d->must_eat > INT_MAX)
		return (1);
	return (0);
}

static int create_philos(t_philo **philos, t_data *d)
{
	long	i;
	
	d->philos = malloc(sizeof(t_philo) * d->n_philo);
	if (!d->philos)
		return (1);
	i = -1;
	while (++i < d->n_philo)
	{
		(*philos)[i].id = i + 1;
		(*philos)[i].last_meal = 0;
		(*philos)[i].eat_count = 0;
		if (pthread_mutex_init(&(*philos)[i].meal_mtx, NULL) == -1)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&(*philos)[i].meal_mtx);
			return (free(d->philos), 1);
		}
		(*philos)[i].fork_l = &d->forks[i];
		if (d->n_philo == 1)
			(*philos)[i].fork_r = NULL;
		else
			(*philos)[i].fork_r = &d->forks[(i + 1) % d->n_philo];
		(*philos)[i].data = d;
	}
	return (0);
}

int	init_data(t_data *d)
{
	long	i;

	d->stop = 0;
	d->forks = NULL;
	d->philos = NULL;
	if (pthread_mutex_init(&d->stop_mtx, NULL) == -1)
		return (1);
	if (pthread_mutex_init(&d->print_mtx, NULL) == -1)
		return (1);
	d->forks = malloc(sizeof(pthread_mutex_t) * d->n_philo);
	if (!d->forks)
		return (1);
	i = -1;
	while (++i < d->n_philo)
	{
		if (pthread_mutex_init(&d->forks[i], NULL) == -1)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&d->forks[i]);
			return (free(d->forks), 1);
		}
	}
	if (create_philos(&d->philos, d))
		return (free(d->forks),1);
	return (0);
}

void	destroy_data(t_data *d)
{
	long	i;

	i = -1;
	while (++i < d->n_philo)
		pthread_mutex_destroy(&d->forks[i]);
	i = -1;
	while (++i < d->n_philo)
		pthread_mutex_destroy(&d->philos[i].meal_mtx);
	pthread_mutex_destroy(&d->print_mtx);
	pthread_mutex_destroy(&d->stop_mtx);
	if (d->forks)
	{
		free(d->forks);
		d->forks = NULL;
	}
	if (d->philos)
	{
		free(d->philos);
		d->philos = NULL;
	}
}
