/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   observer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:46:34 by teando            #+#    #+#             */
/*   Updated: 2025/05/05 00:49:22 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline void	set_stop(t_data *d)
{
	pthread_mutex_lock(&d->stop_mtx);
	d->stop = 1;
	pthread_mutex_unlock(&d->stop_mtx);
}

static inline int	watch_dead(t_data *d)
{
	long	i;
	long	ate;
	int		state;
	long	current_time;
	t_philo	*ph;

	i = -1;
	current_time = now_ms();
	while (++i < d->cf.n_philo)
	{
		ph = &d->philos[i];
		pthread_mutex_lock(&ph->meal_mtx);
		ate = ph->last_meal;
		state = ph->state;
		pthread_mutex_unlock(&ph->meal_mtx);
		if (state == 0 && current_time - ate > d->cf.t_die)
		{
			pthread_mutex_lock(&d->print_mtx);
			printf("%ld %d died\n", current_time - d->start_ts, ph->id);
			set_stop(d);
			pthread_mutex_unlock(&d->print_mtx);
			return (1);
		}
	}
	return (0);
}

static inline int	watch_alive(t_data *d)
{
	long	i;
	long	cnt;
	
	i = -1;
	cnt = 0;
	while (++i < d->cf.n_philo)
	{
		pthread_mutex_lock(&d->philos[i].meal_mtx);
		if (d->philos[i].state == 1)
			cnt++;
		pthread_mutex_unlock(&d->philos[i].meal_mtx);
	}
	if (cnt >= d->cf.n_philo)
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
	if (d->cf.must_eat == 0)
		set_stop(d);
	start_wait(d);
	while (!check_stop(d))
	{
		if (watch_dead(d))
			break ;
		if (d->cf.must_eat > 0 && watch_alive(d))
			break ;
		usleep(100);
	}
	return (NULL);
}
