/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putnbr_fd.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 10:14:16 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/12 15:42:58 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

void	ft_printf_putnbr_fd(int n, int fd)
{
	long	nb;
	int	i;

	i = 0;
	nb = n;
	if (nb < 0)
	{
		ft_putchar_fd('-', fd);
		nb = -nb;
		i++;
	}
	if (nb >= 10)
		i += ft_putnbr_fd(nb / 10, fd);
	ft_putchar_fd((nb % 10 + '0'), fd);
	i++;
	return (1);
}
