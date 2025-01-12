/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 19:03:57 by teando            #+#    #+#             */
/*   Updated: 2024/12/11 19:03:58 by teando           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	smart_sleep(long time_in_ms)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms)
		usleep(100);
}

int	ft_atoi(const char *str)
{
	long	n;
	int		sign;

	n = 0;
	sign = 1;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
		if (*str++ == '-')
			sign = -1;
	while (*str >= '0' && *str <= '9')
		n = n * 10 + (*str++ - '0');
	return ((int)(n * sign));
}

void	print_action(t_data *data, int id, const char *msg)
{
	pthread_mutex_lock(&data->print_mutex);
	if (!data->died && !data->all_ate)
		printf("%ld %d %s\n", get_time() - data->start_time, id, msg);
	pthread_mutex_unlock(&data->print_mutex);
}
