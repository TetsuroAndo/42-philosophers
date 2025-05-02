/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 22:53:35 by teando            #+#    #+#             */
/*   Updated: 2025/05/03 08:12:13 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_cfg
{
	long				n_philo;
	long				t_die;
	long				t_eat;
	long				t_sleep;
	long				must_eat;
}						t_cfg;

typedef struct s_data	t_data;

typedef struct s_philo
{
	int					id;
	long				last_meal;
	long				eat_count;
	pthread_mutex_t		meal_mtx;
	pthread_mutex_t		*fork_l;
	pthread_mutex_t		*fork_r;
	pthread_t			th;
	t_data				*d;
}						t_philo;

typedef struct s_data
{
	t_cfg				cf;
	long				start_ts;
	int					stop;
	pthread_mutex_t		stop_mtx;
	pthread_mutex_t		print_mtx;
	pthread_mutex_t		*forks;
	t_philo				*philos;
}						t_data;

/* main proc */
void					*life(void *arg);
void					*observer(void *arg);

/* data.c */
int						parse_args(t_cfg *c, int ac, char **av);
int						init_data(t_data *d);
void					destroy_data(t_data *d);
void					destory_philos(t_philo *philos, long n_philos);

/* misc.c */
long					now_ms(void);
void					msleep(long ms, t_data *d);
void					put_state(t_philo *p, char *msg);
void					set_stop(t_data *d);
int						check_stop(t_data *d);

/* puterr.c */
size_t					ft_strlen(const char *s);
int						puterr(const char *s);
int						usage(void);
int						puterr_ret(char *msg);

#endif