/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 00:03:42 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 04:08:17 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static inline int	wait_all_childs(t_ctx *gc, pid_t *pids)
{
	pid_t	pid;
	int		status;
	long	finished;
	long	i;

	finished = 0;
	while (finished < gc->cf->n_philo)
	{
		pid = waitpid(-1, &status, 0);
		if (pid < 0)
			break ;
		if (!WIFEXITED(status) || WEXITSTATUS(status))
		{
			i = 0;
			while (i < gc->cf->n_philo)
				kill(pids[i++], SIGTERM);
		}
		++finished;
	}
	return (finished);
}

static inline void	born(t_ctx *gc, const char *fn, const char *pn)
{
	t_ctx	lc;

	lc.id = gc->id;
	lc.cf = gc->cf;
	lc.shm = gc->shm;
	lc.forks_sem = sem_open(fn, 0);
	lc.print_sem = sem_open(pn, 0);
	if (lc.forks_sem == SEM_FAILED || lc.print_sem == SEM_FAILED)
	{
		perror("child sem_open");
		exit(1);
	}
	life(&lc);
}

static inline long	creation(t_ctx *gc, pid_t *pids, const char *fn,
		const char *pn)
{
	long	i;

	gc->shm->start_ts = 0;
	i = -1;
	while (++i < gc->cf->n_philo)
	{
		pids[i] = fork();
		if (pids[i] == 0)
			born(gc, fn, pn);
		else if (pids[i] < 0)
			return (perror("fork"), 1);
	}
	gc->shm->start_ts = now_ms() + 100;
	while (--i >= 0)
		gc->shm->p[i].last_meal = gc->shm->start_ts;
	return (0);
}

int	main(int ac, char **av)
{
	t_ctx		gc;
	pid_t		*pids;
	const char	*fn = "/forks_sem_perm";
	const char	*pn = "/print_sem_perm";

	if (parse_args(&gc.cf, ac, av))
		return (puterr_ret("Bad arguments"));
	if (init_data(&gc, pids, fn, pn))
		return (puterr_ret("Initialize Philosophers failed"));
	if (creation(&gc, pids, fn, pn))
	{
		destroy(&gc, pids, fn, pn);
		return (puterr_ret("Simulation failed"));
	}
	wait_all_childs(&gc, pids);
	destroy(&gc, pids, fn, pn);
	return (EXIT_SUCCESS);
}
