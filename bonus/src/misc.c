/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:42:27 by teando            #+#    #+#             */
/*   Updated: 2025/05/03 19:43:19 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

long	now_ms(void)
{
	struct timeval	t;

	t = (struct timeval){0};
	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000L + t.tv_usec / 1000);
}

void	msleep(long ms)
{
	long	target;

	target = now_ms() + ms;
	while (now_ms() < target)
		usleep(100);
}

void	put_state(t_ctx *c, long id, char *msg)
{
	sem_wait(c->sem.print_sem);
	printf("%ld %ld %s\n", now_ms() - c->start_ts, id, msg);
	sem_post(c->sem.print_sem);
}

void	write_meal(t_ctx *c, t_philo *p)
{
	long	current_time;

	current_time = now_ms();
	sem_wait(c->sem.meal_sem);
	p->last_meal = current_time;
	p->eat_count++;
	sem_post(c->sem.meal_sem);
	put_state(c, p->id, "is eating");
}

void	watch_died(t_ctx *c, t_philo *p)
{
	long	current_time;
	long	last_meal_time;

	current_time = now_ms();
	sem_wait(c->sem.meal_sem);
	last_meal_time = p->last_meal;
	sem_post(c->sem.meal_sem);
	if (current_time - last_meal_time >= c->cf.t_die)
	{
		sem_wait(c->sem.print_sem);
		printf("%ld %ld died\n", current_time - c->start_ts, p->id);
		exit(1);
	}
}
