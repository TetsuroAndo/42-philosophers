/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 14:51:26 by teando            #+#    #+#             */
/*   Updated: 2025/05/05 00:14:06 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static inline void	*ph_guardian(void *arg)
{
	t_obs_arg	*obs;
	t_ctx		*c;
	t_philo		*self;

	obs = (t_obs_arg *)arg;
	c = obs->c;
	self = obs->self;
	while (now_ms() < c->start_ts)
		usleep(100);
	while (1)
	{
		watch_died(c, self);
		usleep(100);
	}
	return (NULL);
}

static inline void	ph_one(t_ctx *c, t_philo *self)
{
	while (now_ms() < c->start_ts)
		usleep(100);
	sem_wait(c->sem.forks_sem);
	put_state(c, self->id, "has taken a fork");
	msleep(c->cf.t_die);
	sem_post(c->sem.forks_sem);
	watch_died(c, self);
	exit(1);
}

static inline void	eat(t_ctx *c, t_philo *self)
{
	sem_wait(c->sem.dining_sem);
	sem_wait(c->sem.forks_sem);
	put_state(c, self->id, "has taken a fork");
	sem_wait(c->sem.forks_sem);
	put_state(c, self->id, "has taken a fork");
	write_meal(c, self);
	msleep(c->cf.t_eat);
	sem_post(c->sem.forks_sem);
	sem_post(c->sem.forks_sem);
	sem_post(c->sem.dining_sem);
	if (c->cf.must_eat >= 0 && self->eat_count >= c->cf.must_eat)
		exit(0);
}

void	life(t_ctx *c, t_philo *self)
{
	t_obs_arg	arg;
	pthread_t	th;

	if (c->cf.n_philo == 1)
		ph_one(c, self);
	arg.c = c;
	arg.self = self;
	if (pthread_create(&th, NULL, ph_guardian, &arg))
		puterr_exit("pthread_create");
	pthread_detach(th);
	while (now_ms() < c->start_ts)
		usleep(100);
	usleep((self->id - 1) * c->cf.t_eat);
	while (1)
	{
		put_state(c, self->id, "is thinking");
		eat(c, self);
		put_state(c, self->id, "is sleeping");
		msleep(c->cf.t_sleep);
	}
	exit(0);
}
