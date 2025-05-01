/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 14:51:02 by teando            #+#    #+#             */
/*   Updated: 2025/05/02 08:08:04 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <fcntl.h>
# include <limits.h>
# include <pthread.h>
# include <semaphore.h>
# include <signal.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <unistd.h>

typedef struct s_philo
{
	long		id;
	long		last_meal;
	long		eat_count;
}				t_philo;

typedef struct s_cfg
{
	long		n_philo;
	long		t_die;
	long		t_eat;
	long		t_sleep;
	long		must_eat;
}				t_cfg;

typedef struct s_sem_set
{
	const char	*fn;
	const char	*pn;
	const char	*dn;
	sem_t		*forks_sem;
	sem_t		*print_sem;
	sem_t		*dining_sem;
}				t_sem_set;

typedef struct s_ctx
{
	long		start_ts;
	t_cfg		cf;
	t_sem_set	sem;
}				t_ctx;

typedef struct s_obs_arg
{
	t_ctx		*c;
	t_philo		*self;
}				t_obs_arg;

/* main process */
void			life(t_ctx *c, t_philo *self);

/* data.c */
int				parse_args(t_cfg *cf, int ac, char **av);
int				init_data(t_ctx *c, pid_t **pids);
void			sem_unlink_all(t_sem_set *sem);
void			destroy(t_ctx *c, pid_t *pids);

/* misc.c */
long			now_ms(void);
void			msleep(long ms);
void			put_state(t_ctx *c, long id, char *msg);

/* err.c */
int				puterr(const char *s);
void			usage(void);
int				puterr_ret(char *msg);
void			puterr_exit(char *msg);

#endif
