/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 00:03:42 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 06:20:30 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline int	wait_all_childs(t_ctx *c, pid_t *pids)
{
	pid_t	pid;
	int		status;
	long	finished;
	long	i;

	finished = 0;
	while (finished < c->cf.n_philo)
	{
		pid = waitpid(-1, &status, 0);
		if (pid < 0)
			break ;
		if (!WIFEXITED(status) || WEXITSTATUS(status))
		{
			i = 0;
			while (i < c->cf.n_philo)
				kill(pids[i++], SIGTERM);
		}
		++finished;
	}
	return (finished);
}

static inline void	born(t_ctx *c, long id, const char *fn, const char *pn)
{
	t_ctx	lc;

	lc.start_ts = c->start_ts;
	lc.cf = c->cf;
	lc.forks_sem = sem_open(fn, 0);
	lc.print_sem = sem_open(pn, 0);
	lc.p = &c->p[id - 1];
	if (lc.forks_sem == SEM_FAILED || lc.print_sem == SEM_FAILED)
		puterr_exit("child sem_open");
	life(&lc, id);
}

static inline long	creation(t_ctx *c, pid_t *pids, const char *fn,
		const char *pn)
{
	long	i;

	c->start_ts = now_ms() + 1000;
	i = -1;
	while (++i < c->cf.n_philo)
	{
		pids[i] = fork();
		if (pids[i] < 0)
			puterr_exit("fork");
		if (pids[i] == 0)
			born(c, i + 1, fn, pn);
	}
	return (0);
}

int	main(int ac, char **av)
{
	t_ctx		c;
	pid_t		*pids;
	const char	*fn = "/forks_sem_perm";
	const char	*pn = "/print_sem_perm";

	if (parse_args(&c.cf, ac, av))
		return (puterr_ret("Bad arguments"));
	if (init_data(&c, &pids, fn, pn))
		return (puterr_ret("Initialize Philosophers failed"));
	if (creation(&c, pids, fn, pn))
	{
		destroy(&c, pids, fn, pn);
		return (puterr_ret("Simulation failed"));
	}
	wait_all_childs(&c, pids);
	destroy(&c, pids, fn, pn);
	return (EXIT_SUCCESS);
}
