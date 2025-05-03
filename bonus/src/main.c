/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 00:03:42 by teando            #+#    #+#             */
/*   Updated: 2025/05/03 09:02:27 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static inline void	wait_all_childs(t_ctx *c, pid_t *pids)
{
	pid_t	pid;
	int		status;
	long	i;

	i = -1;
	while (++i < c->cf.n_philo)
	{
		pid = waitpid(-1, &status, 0);
		if (pid < 0)
			break ;
		if (!WIFEXITED(status) || WEXITSTATUS(status))
		{
			i = -1;
			while (++i < c->cf.n_philo)
				kill(pids[i], SIGTERM);
			sem_post(c->sem.print_sem);
			break ;
		}
	}
}

static inline void	born(t_ctx *gc, long id)
{
	t_ctx	lc;
	t_philo	lp;

	lp.id = id;
	lp.last_meal = gc->start_ts;
	lp.eat_count = 0;
	lc.start_ts = gc->start_ts;
	lc.cf = gc->cf;
	lc.sem.forks_sem = sem_open(gc->sem.fn, 0);
	lc.sem.print_sem = sem_open(gc->sem.pn, 0);
	lc.sem.dining_sem = sem_open(gc->sem.dn, 0);
	lc.sem.meal_sem = sem_open(gc->sem.mn, 0);
	if (lc.sem.forks_sem == SEM_FAILED || lc.sem.print_sem == SEM_FAILED
		|| lc.sem.dining_sem == SEM_FAILED || lc.sem.meal_sem == SEM_FAILED)
		puterr_exit("child sem_open");
	life(&lc, &lp);
}

static inline long	creation(t_ctx *c, pid_t *pids)
{
	long	i;

	c->start_ts = now_ms() + 1000;
	i = -1;
	while (++i < c->cf.n_philo)
	{
		pids[i] = fork();
		if (pids[i] < 0)
			return (puterr_ret("fork"));
		if (pids[i] == 0)
			born(c, i + 1);
	}
	return (0);
}

int	main(int ac, char **av)
{
	t_ctx	c;
	pid_t	*pids;

	c.sem.fn = "/forks_sem_perm";
	c.sem.pn = "/print_sem_perm";
	c.sem.dn = "/dining_sem_perm";
	c.sem.mn = "/meal_sem_perm";
	if (parse_args(&c.cf, ac, av))
		return (puterr_ret("Bad arguments"));
	if (init_data(&c, &pids))
	{
		destroy(&c, NULL);
		return (puterr_ret("Initialize Philosophers failed"));
	}
	if (creation(&c, pids))
	{
		destroy(&c, pids);
		return (puterr_ret("Simulation failed"));
	}
	wait_all_childs(&c, pids);
	destroy(&c, pids);
	return (EXIT_SUCCESS);
}
