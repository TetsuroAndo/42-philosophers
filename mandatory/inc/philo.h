/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:06:12 by teando            #+#    #+#             */
/*   Updated: 2024/12/14 11:56:43 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_philo	t_philo;
typedef struct s_data	t_data;

typedef struct s_data
{
	int					num_philos;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	int					must_eat_count;
	int					died;
	int					all_ate;
	long				start_time;
	pthread_mutex_t		*forks;
	pthread_mutex_t		print_mutex;
	pthread_mutex_t		meal_check;
	t_philo				*philos;
}						t_data;

typedef struct s_philo
{
	int					id;
	int					eaten_meals;
	long				last_meal;
	pthread_t			thread_id;
	t_data				*data;
	pthread_mutex_t		*left_fork;
	pthread_mutex_t		*right_fork;
}						t_philo;

// init.c
int						init_data(t_data *data, int ac, char **av);
int						init_philos(t_data *data);
void					clean_up(t_data *data);

// simulation.c
int						start_simulation(t_data *data);

// actions.c
void					philo_eat(t_philo *philo);
void					philo_sleep_think(t_philo *philo);

// monitor.c
void					*monitor_death(void *arg);

// utils.c
long					get_time(void);
void					smart_sleep(long time_in_ms);
int						ft_atoi(const char *str);
void					print_action(t_data *data, int id, const char *msg);

#endif
