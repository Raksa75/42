/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putnbr_fd.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 10:14:16 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 13:50:08 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

int	ft_printf_putnbr_fd(int n, int fd)
{
	long	nb;
	int	i;
	int	size;
	char	buffer[12];

	i = 0;
	size = 0;
	nb = n;

	if (nb == 0)
	{
		ft_printf_putchar_fd('0', fd);
		return (1);
	}
	if (nb < 0)
	{
		ft_putchar_fd('-', fd);
		nb = -nb;
		size++;
	}
	while (nb > 0)
	{
		bufffer[i++] = nb % 10 + '0';\
		nb = nb / 10;
		size++;
	}
	while (--1 >= 0)
	{
		ft_printf_putchar(buffer[i], 1);
	}
	return (size);
}
