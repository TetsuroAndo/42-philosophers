/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:49:40 by teando            #+#    #+#             */
/*   Updated: 2025/05/03 08:43:44 by teando           ###   ########.fr       */
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
	if (p->d->cf.n_philo == 1)
		return (1);
	pthread_mutex_lock(second);
	put_state(p, "has taken a fork");
	return (0);
}

static inline int	philo_eat(t_philo *p)
{
	if (take_forks(p))
		return (1);
	put_state(p, "is eating");
	set_last_meal(p, now_ms());
	msleep(p->d->cf.t_eat, p->d);
	pthread_mutex_unlock(p->fork_l);
	pthread_mutex_unlock(p->fork_r);
	if (p->d->cf.must_eat >= 0 && p->eat_count >= p->d->cf.must_eat)
		return (1);
	return (0);
}

void	*life(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	start_wait(p->d);
	if (p->d->cf.n_philo % 2 == 1)
		usleep((p->id - 1) / 2 * p->d->cf.t_eat);
	else if (p->id % 2 == 0)
		usleep(p->d->cf.t_eat * 0.8);
	while (!check_stop(p->d))
	{
		put_state(p, "is thinking");
		if (philo_eat(p))
			break ;
		put_state(p, "is sleeping");
		msleep(p->d->cf.t_sleep, p->d);
	}
	return (NULL);
}
