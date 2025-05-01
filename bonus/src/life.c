/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 14:51:26 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 06:27:41 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*ph_guardian(void *arg)
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
		if (now_ms() - self->last_meal >= c->cf.t_die)
		{
			sem_wait(c->print_sem);
			printf("%ld %ld died\n", now_ms() - c->start_ts, self->id);
			sem_post(c->print_sem);
			kill(0, SIGTERM);
			exit(1);
		}
		usleep(100);
	}
	return (NULL);
}

static inline void	put_guardian_proc(t_ctx *c, t_philo *self)
{
	pthread_t	th;
	t_obs_arg	arg;

	arg.c = c;
	arg.self = self;
	if (pthread_create(&th, NULL, ph_guardian, &arg))
		puterr_exit("pthread_create");
	pthread_detach(th);
}

static inline int	eat(t_ctx *c, t_philo *self, long id)
{
	long	must_eat;

	must_eat = c->cf.must_eat;
	if (must_eat >= 0 && self->eat_count >= must_eat)
	{
		kill(0, SIGTERM);
		exit(0);
	}
	sem_wait(c->forks_sem);
	put_state(c, id, "has taken a fork");
	if (c->cf.n_philo <= 1)
		return (1);
	sem_wait(c->forks_sem);
	put_state(c, id, "has taken a fork");
	put_state(c, id, "is eating");
	self->last_meal = now_ms();
	self->eat_count++;
	msleep(c->cf.t_eat);
	sem_post(c->forks_sem);
	sem_post(c->forks_sem);
	return (0);
}

void	life(t_ctx *c, long id)
{
	t_philo	*self;

	self = c->p;
	self->last_meal = c->start_ts;
	put_guardian_proc(c, self);
	while (now_ms() < c->start_ts)
		usleep(100);
	while (1)
	{
		put_state(c, id, "is thinking");
		if (eat(c, self, id))
			break ;
		put_state(c, id, "is sleeping");
		msleep(c->cf.t_sleep);
	}
	exit(0);
}
