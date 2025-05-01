/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 14:51:26 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 05:55:13 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline void	put_guardian_proc(t_ctx *c, t_philo *self)
{
	pid_t pid;

	pid = fork();
    if (pid < 0)
        puterr_exit("fork");
    if (pid == 0)
    {
        while (!c->start_ts || now_ms() < self->last_meal)
            usleep(100);
        while (1)
        {
            if (now_ms() - self->last_meal >= c->cf->t_die)
            {
                sem_wait(c->print_sem);
                printf("%ld %ld died\n", now_ms() - c->start_ts, self->id);
                sem_post(c->print_sem);
                kill(0, SIGTERM);
                exit(1);
            }
            usleep(100);
        }
    }
}

static inline void	eat(t_ctx *c, t_philo *self, long id)
{
	long	must_eat;

	must_eat = c->cf->must_eat;
	if (must_eat >= 0 && self->eat_count >= must_eat)
	{
		kill(0, SIGTERM);
		exit(0);
	}
	sem_wait(c->forks_sem);
	put_state(c, id, "has taken a fork");
	sem_wait(c->forks_sem);
	put_state(c, id, "has taken a fork");
	put_state(c, id, "is eating");
	self->last_meal = now_ms();
	self->eat_count++;
	msleep(c->cf->t_eat);
	sem_post(c->forks_sem);
	sem_post(c->forks_sem);
}

void	life(t_ctx *c, long id)
{
	t_philo	*self;

	self = c->p;
	put_guardian_proc(c, self);
	if (id % 2 == 0)
		usleep(100);
	while (!c->start_ts || now_ms() < self->last_meal)
		usleep(100);
	while (1)
	{
		put_state(c, id, "is thinking");
		eat(c, self, id);
		put_state(c, id, "is sleeping");
		msleep(c->cf->t_sleep);
	}
	exit(0);
}
