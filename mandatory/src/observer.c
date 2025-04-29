/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   observer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:46:34 by teando            #+#    #+#             */
/*   Updated: 2025/04/30 07:43:01 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline int	watch_dead(t_data *d)
{
	long	i;
	long	ate;
	t_philo	*ph;

	i = -1;
	while (++i < d->n_philo)
	{
		ph = &d->philos[i];
		pthread_mutex_lock(&ph->meal_mtx);
		ate = ph->last_meal;
		pthread_mutex_unlock(&ph->meal_mtx);
		if (now_ms() - ate > d->t_die)
		{
			pthread_mutex_lock(&d->print_mtx);
			printf("%ld %d died\n", now_ms() - d->start_ts, ph->id);
			pthread_mutex_unlock(&d->print_mtx);
			set_stop(d);
			return (1);
		}
	}
	return (0);
}

static inline int	watch_alive(t_data *d)
{
	long	i;
	long	cnt;

	cnt = 0;
	i = -1;
	while (++i < d->n_philo)
	{
		pthread_mutex_lock(&d->philos[i].meal_mtx);
		if (d->philos[i].eat_count >= d->must_eat)
			cnt++;
		pthread_mutex_unlock(&d->philos[i].meal_mtx);
	}
	if (cnt >= d->n_philo)
	{
		set_stop(d);
		return (1);
	}
	return (0);
}

void	*observer(void *arg)
{
	t_data	*d;

	d = (t_data *)arg;
	while (!d->stop)
	{
		if (watch_dead(d))
			break ;
		if (d->must_eat != -1 && watch_alive(d))
			break ;
	}
	return (NULL);
}
