/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	ft_strcmp(const char *a, const char *b)
{
	int	i;

	i = 0;
	while (a[i] && a[i] == b[i])
		i++;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

void	put_str(int fd, const char *s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	if (write(fd, s, len) < 0)
		return ;
}

void	put_nbr(int fd, long n)
{
	char	c;

	if (n < 0)
	{
		put_str(fd, "-");
		n = -n;
	}
	if (n >= 10)
		put_nbr(fd, n / 10);
	c = '0' + n % 10;
	if (write(fd, &c, 1) < 0)
		return ;
}
