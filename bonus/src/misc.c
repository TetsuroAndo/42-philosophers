/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:42:27 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 08:22:05 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

long	now_ms(void)
{
	struct timeval	t;

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
