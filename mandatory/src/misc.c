/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:42:27 by teando            #+#    #+#             */
/*   Updated: 2025/05/03 08:48:23 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	now_ms(void)
{
	struct timeval	t;

	t = (struct timeval){0};
	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000L + t.tv_usec / 1000);
}

void	msleep(long ms, t_data *d)
{
	long	target;

	target = now_ms() + ms;
	while (!check_stop(d) && now_ms() < target)
		usleep(10);
}

void	put_state(t_philo *p, char *msg)
{
	pthread_mutex_lock(&p->d->print_mtx);
	if (!check_stop(p->d))
		printf("%ld %d %s\n", now_ms() - p->d->start_ts, p->id, msg);
	pthread_mutex_unlock(&p->d->print_mtx);
}

int	check_stop(t_data *d)
{
	int	ret;

	pthread_mutex_lock(&d->stop_mtx);
	ret = d->stop;
	pthread_mutex_unlock(&d->stop_mtx);
	return (ret);
}

void	start_wait(t_data *d)
{
	long	current_time;
	int		waiting;

	waiting = 0;
	while (!check_stop(d))
	{
		current_time = now_ms();
		pthread_mutex_lock(&d->stop_mtx);
		waiting = (d->start_ts && current_time >= d->start_ts);
		pthread_mutex_unlock(&d->stop_mtx);
		if (waiting)
			break ;
		usleep(100);
	}
}
