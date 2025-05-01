/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 14:51:26 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 04:34:32 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline void	guardian(void *arg)
{
	t_ctx		*c;
	t_obs_arg	*o;
	t_philo		*self;

	o = (t_obs_arg *)arg;
	c = o->c;
	self = o->self;
	while (!c->shm->start_ts || now_ms() < self->last_meal)
		usleep(100);
	while (1)
	{
		if (now_ms() - self->last_meal >= c->cf->t_die)
		{
			put_state(c, "died");
			exit(1);
		}
	}
	return (NULL);
}

static inline void	put_a_guardian(t_ctx *c, t_philo *self)
{
	t_obs_arg	obs_arg;
	pthread_t	obs_th;

	obs_arg.c = c;
	obs_arg.self = self;
	if (pthread_create(&obs_th, NULL, observer, &obs_arg))
		exit(1);
	pthread_detach(obs_th);
}

static inline void	eat(t_ctx *c, t_philo *self)
{
	long	must_eat;

	must_eat = c->cf->must_eat;
	if (must_eat >= 0 && self->eat_count >= must_eat)
		exit(0);
	sem_wait(c->forks_sem);
	put_state(c, "has taken a fork");
	sem_wait(c->forks_sem);
	put_state(c, "has taken a fork");
	put_state(c, "is eating");
	self->last_meal = now_ms();
	self->eat_count++;
	msleep(c->cf->t_eat, c);
	sem_post(c->forks_sem);
	sem_post(c->forks_sem);
}

void	life(t_ctx *c)
{
	t_philo	*self;

	self = &c->shm->p[c->id - 1];
	put_a_guardian(c, self);
	while (!c->shm->start_ts || now_ms() < self->last_meal)
		usleep(100);
	while (!check_stop(c->shm))
	{
		put_state(c, "is thinking");
		eat(c, self);
		put_state(c, "is sleeping");
		msleep(c->cf->t_sleep, c);
	}
	exit(0);
}
