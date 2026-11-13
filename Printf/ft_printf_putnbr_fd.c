/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putnbr_fd.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 10:14:16 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 14:42:25 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_printsize(long nb)
{
	int	i;

	i = 0;
	while (nb >= 10)
	{
		nb /= 10;
		i++;
	}
	return (i)
}

int	ft_printf_putnbr_fd(int n, int fd)
{
	long	nb;
	int	i;

	nb = n;
	if (nb < 0)
	{
		ft_printf_putchar_fd('-', fd);
		nb = -nb;
	}
	if (nb >= 10)
		ft_printf_putnbr_fd(nb / 10, fd);
	ft_printf_putchar_fd((nb % 10 + '0'), fd);
	return (i + ft_printsize(nb));
}
