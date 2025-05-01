/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 14:51:26 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 08:22:17 by teando           ###   ########.fr       */
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
	while (now_ms() < self->last_meal)
		usleep(100);
	while (1)
	{
		if (now_ms() - self->last_meal >= c->cf.t_die)
		{
			sem_wait(c->sem.print_sem);
			printf("%ld %ld died\n", now_ms() - c->start_ts, self->id);
			sem_post(c->sem.print_sem);
			exit(1);
		}
		usleep(100);
	}
	return (NULL);
}

static inline void	eat(t_ctx *c, t_philo *self)
{
	sem_wait(c->sem.dining_sem);
	sem_wait(c->sem.forks_sem);
	put_state(c, self->id, "has taken a fork");
	if (c->cf.n_philo <= 1)
	{
		sem_post(c->sem.forks_sem);
		sem_post(c->sem.dining_sem);
		exit(1);
	}
	sem_wait(c->sem.forks_sem);
	put_state(c, self->id, "has taken a fork");
	put_state(c, self->id, "is eating");
	self->last_meal = now_ms();
	self->eat_count++;
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

	arg.c = c;
	arg.self = self;
	if (pthread_create(&th, NULL, ph_guardian, &arg))
		puterr_exit("pthread_create");
	pthread_detach(th);
	while (now_ms() < self->last_meal)
		usleep(100);
	while (1)
	{
		put_state(c, self->id, "is thinking");
		eat(c, self);
		put_state(c, self->id, "is sleeping");
		msleep(c->cf.t_sleep);
	}
	exit(0);
}
