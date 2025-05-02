/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   observer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:46:34 by teando            #+#    #+#             */
/*   Updated: 2025/05/03 07:54:18 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline int	watch_dead(t_data *d)
{
	long	i;
	long	ate;
	long	current_time;
	t_philo	*ph;

	i = -1;
	current_time = now_ms();
	while (++i < d->cf.n_philo)
	{
		ph = &d->philos[i];
		pthread_mutex_lock(&ph->meal_mtx);
		ate = ph->last_meal;
		if (current_time - ate > d->cf.t_die)
		{
			pthread_mutex_lock(&d->print_mtx);
			printf("%ld %d died\n", current_time - d->start_ts, ph->id);
			set_stop(d);
			pthread_mutex_unlock(&d->print_mtx);
			pthread_mutex_unlock(&ph->meal_mtx);
			return (1);
		}
		pthread_mutex_unlock(&ph->meal_mtx);
	}
	return (0);
}

void	*observer(void *arg)
{
	t_data	*d;
	long	wait_st;

	d = (t_data *)arg;
	while (1)
	{
		pthread_mutex_lock(&d->stop_mtx);
		wait_st = d->start_ts;
		pthread_mutex_unlock(&d->stop_mtx);
		if (wait_st && now_ms() >= wait_st)
			break ;
		usleep(100);
	}
	while (!check_stop(d))
	{
		if (watch_dead(d))
			break ;
		usleep(100);
	}
	return (NULL);
}
