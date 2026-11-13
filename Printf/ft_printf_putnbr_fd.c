/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putnbr_fd.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 10:14:16 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 17:13:12 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

int	ft_printf_putnbr_fd(long n, int fd)
{
	int		i;

	i = 0;
	if (n < 0)
	{
		i += ft_printf_putchar_fd('-', fd);
		n = -n;
	}
	if (n >= 10)
		i += ft_printf_putnbr_fd(n / 10, fd);
	i += ft_printf_putchar_fd((n % 10 + '0'), fd);
	return (i);
}
