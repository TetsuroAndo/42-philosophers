/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 02:43:26 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 04:07:21 by teando           ###   ########.fr       */
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

int	init_data(t_ctx *c, pid_t *pids, const char *fn, const char *pn)
{
	sem_unlink_all(fn, pn);
	c->forks_sem = sem_open(fn, O_CREAT | O_EXCL, 0600, c->cf->n_philo);
	c->print_sem = sem_open(pn, O_CREAT | O_EXCL, 0600, 1);
	if (c->forks_sem == SEM_FAILED || c->print_sem == SEM_FAILED)
		return (perror("sem_open"), 1);
	pids = malloc(sizeof(pid_t) * c->cf->n_philo);
	if (!pids)
		return (perror("malloc"), 1);
	return (0);
}

void	sem_unlink_all(const char *fn, const char *pn)
{
	sem_unlink(fn);
	sem_unlink(pn);
}

void	destroy(t_ctx *c, pid_t *pids, const char *fn, const char *pn)
{
	free(pids);
	sem_unlink_all(fn, pn);
	sem_close(c->forks_sem);
	sem_close(c->print_sem);
}
