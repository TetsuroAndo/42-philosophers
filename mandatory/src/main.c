/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 18:35:15 by teando            #+#    #+#             */
/*   Updated: 2025/02/07 23:11:04 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	usage(void)
{
	printf("Usage: ./philo <num_of_philos> <time_to_die> <time_to_eat> "
			"<time_to_sleep> [num_of_must_eat]\n\n");
	printf("num_of_philos       number of philosophers and nunber of forks\n");
	printf("time_to_die [ms]    the time in when they must start eating\n");
	printf("time_to_eat [ms]    the time it takes them to eat\n");
	printf("time_to_sleep [ms]  the amount of sleep they need\n");
	printf("num_of_must_eat     number of times each philosopher must eat\n");
	return (1);
}

int	print_error(char *msg)
{
	printf("Error: %s\n", msg);
	return (1);
}

int	main(int ac, char **av)
{
	t_info	info;
	t_philo	*philos;
	int		i;

	if (ac < 5 || ac > 6)
		return (usage());
	if (init_info(&info, ac, av) != 0)
		return (usage());
	/* 哲学者スレッドの初期化 */
	if (init_philos(&philos, &info) != 0)
	{
		free(info.forks);
		return (print_error("init_philos failed"));
	}
	/* シミュレーション開始 */
	start_simulation(philos, &info);
	/*
	** 終了処理
	** - フォークミューテックスの破棄
	** - 哲学者配列のfree
	*/
	i = -1;
	while (++i < info.nb_philo)
		pthread_mutex_destroy(&info.forks[i]);
	pthread_mutex_destroy(&info.print_lock);
	free(info.forks);
	free(philos);
}
