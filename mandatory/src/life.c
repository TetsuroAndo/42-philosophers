/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:49:40 by teando            #+#    #+#             */
/*   Updated: 2025/05/01 13:08:41 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline void	set_last_meal(t_philo *p, long ts)
{
	pthread_mutex_lock(&p->meal_mtx);
	p->last_meal = ts;
	p->eat_count++;
	pthread_mutex_unlock(&p->meal_mtx);
}

static inline int	take_forks(t_philo *p)
{
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	if (p->id % 2)
	{
		first = p->fork_l;
		second = p->fork_r;
	}
	else
	{
		first = p->fork_r;
		second = p->fork_l;
	}
	pthread_mutex_lock(first);
	put_state(p, "has taken a fork");
	if (p->data->n_philo == 1)
		return (1);
	pthread_mutex_lock(second);
	put_state(p, "has taken a fork");
	return (0);
}

static inline void	drop_forks(t_philo *p)
{
	pthread_mutex_unlock(p->fork_l);
	if (p->data->n_philo > 1)
		pthread_mutex_unlock(p->fork_r);
}

static inline int	philo_eat(t_philo *p)
{
	if (take_forks(p))
		return (1);
	put_state(p, "is eating");
	set_last_meal(p, now_ms());
	msleep(p->data->t_eat, p->data);
	drop_forks(p);
	if (p->data->must_eat > 0 && p->eat_count >= p->data->must_eat)
	{
		return (1);
	}
	return (0);
}

void	*life(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	while (p->data->start_ts == 0)
		usleep(10);
	while (now_ms() < p->data->start_ts)
		usleep(10);
	while (!check_stop(p->data))
	{
		put_state(p, "is thinking");
		if (philo_eat(p))
			break ;
		put_state(p, "is sleeping");
		msleep(p->data->t_sleep, p->data);
	}
	return (NULL);
}
