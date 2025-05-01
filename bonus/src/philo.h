/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 14:51:02 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 04:37:54 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <errno.h>
# include <limits.h>
# include <pthread.h>
# include <semaphore.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>

typedef struct s_philo
{
	long		last_meal;
	long		eat_count;
}				t_philo;

typedef struct s_shared
{
	long		start_ts;
	int			stop;
	t_philo		*p;
}				t_shared;

typedef struct s_cfg
{
	long		n_philo;
	long		t_die;
	long		t_eat;
	long		t_sleep;
	long		must_eat;
}				t_cfg;

typedef struct s_ctx
{
	long		id;
	t_cfg		*cf;
	t_shared	*shm;
	sem_t		*forks_sem;
	sem_t		*print_sem;
}				t_ctx;

typedef struct s_obs_arg
{
	t_ctx		*c;
	t_philo		*self;
}				t_obs_arg;

/* main process */

void			*observer(void *arg);

/* data.c */
int				parse_args(t_cfg *cf, int ac, char **av);
int				init_data(t_ctx *c, pid_t *pids, const char *fn,
					const char *pn);
void			sem_unlink_all(const char *fn, const char *pn);
void			destroy(t_ctx *c, pid_t *pids, const char *fn, const char *pn);

/* misc.c */
long			now_ms(void);
void			msleep(long ms, t_ctx *c);
void			put_state(t_ctx *c, char *msg);
void			set_stop(t_ctx *c);
int				check_stop(t_shared *shm);

/* err.c */
int				puterr(const char *s);
void			usage(void);
int				puterr_ret(char *msg);
void			puterr_exit(char *msg);

#endif
