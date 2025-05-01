/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:42:27 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 04:37:55 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	now_ms(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000L + t.tv_usec / 1000);
}

void	msleep(long ms, t_ctx *c)
{
	long	target;

	target = now_ms() + ms;
	while (!check_stop(c->shm) && now_ms() < target)
		usleep(100);
}

void	put_state(t_ctx *c, char *msg)
{
	sem_wait(c->print_sem);
	if (!check_stop(c->shm))
		printf("%ld %d %s\n", now_ms() - c->shm->start_ts, c->id, msg);
	sem_post(c->print_sem);
}

void	set_stop(t_ctx *c)
{
	c->shm->stop = 1;
}

int	check_stop(t_shared *shm)
{
	return (shm->stop);
}
