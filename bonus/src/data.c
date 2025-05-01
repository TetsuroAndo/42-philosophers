/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:43:26 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 07:47:54 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static long	ft_atol(const char *nptr)
{
	unsigned long	cutoff;
	int				neg;
	int				tmp;
	unsigned long	res;

	neg = 0;
	res = 0;
	while (*nptr == ' ' || (*nptr >= 9 && *nptr <= 13))
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		neg = *nptr++ == '-';
	cutoff = (unsigned long)LONG_MAX ^ -neg;
	while (*nptr >= '0' && *nptr <= '9')
	{
		res *= 10;
		tmp = *nptr++ - '0';
		if (res > cutoff || cutoff - res < (unsigned long)tmp)
			return (cutoff);
		res += tmp;
	}
	if (neg)
		return (-res);
	return (res);
}

int	parse_args(t_cfg *cf, int ac, char **av)
{
	if (ac < 5 || ac > 6)
		usage();
	cf->n_philo = ft_atol(av[1]);
	cf->t_die = ft_atol(av[2]);
	cf->t_eat = ft_atol(av[3]);
	cf->t_sleep = ft_atol(av[4]);
	cf->must_eat = -1;
	if (ac == 6)
		cf->must_eat = ft_atol(av[5]);
	if (cf->n_philo <= 0 || cf->t_die <= 0 || cf->t_eat <= 0 || cf->t_sleep <= 0
		|| cf->must_eat < -1)
		return (1);
	if (cf->n_philo > INT_MAX || cf->t_die > INT_MAX || cf->t_eat > INT_MAX
		|| cf->t_sleep > INT_MAX || cf->must_eat > INT_MAX)
		return (1);
	return (0);
}

int	init_data(t_ctx *c, pid_t **pids)
{
	int	 philos;
	int  dining;

	sem_unlink_all(&c->sem);
	philos = c->cf.n_philo;
	dining = philos - 1;
	c->sem.forks_sem = sem_open(c->sem.fn, O_CREAT | O_EXCL, 0600, philos);
	c->sem.print_sem = sem_open(c->sem.pn, O_CREAT | O_EXCL, 0600, 1);
	c->sem.dining_sem = sem_open(c->sem.dn, O_CREAT | O_EXCL, 0600, dining);
	if (c->sem.forks_sem == SEM_FAILED || c->sem.print_sem == SEM_FAILED || c->sem.dining_sem == SEM_FAILED)
		return (puterr_ret("sem_open failed"));
	*pids = malloc(sizeof(pid_t) * philos);
	if (!*pids)
		return (puterr_ret("Malloc failed"));
	return (0);
}

void	sem_unlink_all(t_sem_set *sem)
{
	sem_unlink(sem->fn);
	sem_unlink(sem->pn);
	sem_unlink(sem->dn);
}

void	destroy(t_ctx *c, pid_t *pids)
{
	sem_close(c->sem.forks_sem);
	sem_close(c->sem.print_sem);
	sem_close(c->sem.dining_sem);
	sem_unlink_all(&c->sem);
	free(pids);
}
