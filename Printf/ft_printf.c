/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 14:02:38 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 17:12:51 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

int	ft_printf(const char *input, ...)
{
	va_list	args;
	int		i;
	int		total;

	va_start(args, input);
	total = 0;
	i = 0;
	while (input[i] != '\0')
	{
		if (input[i] == '%')
		{
			if (input[i + 1] == 'c')
				total = total + ft_printf_putchar_fd(va_arg(args, int), 1);
			else if (input[i + 1] == 's')
				total += ft_printf_putstr_fd(va_arg(args, char *), 1);
			else if (input[i + 1] == 'p')
				total += ft_printf_putpoint(va_arg(args, char *));
			else if (input[i + 1] == 'd' || input[i+ 1] == 'i')
				total += ft_printf_putnbr_fd(va_arg(args, int), 1);
			else if (input[i + 1] == 'u')
				total += ft_printf_putnbr_fd(va_arg(args, unsigned int), 1);
			else if (input[i + 1] == 'x')
				total += ft_convertbase(va_arg(args, unsigned int), 0);
			else if (input[i + 1] == 'X')
				total += ft_convertbase(va_arg(args, unsigned int), 1);
			else if (input[i + 1] == '%')
				total += ft_printf_putchar_fd('%', 1);
			else
				total += ft_printf_putchar_fd('%', 1);
			i = i + 2;
		}
		else
		{
			total += ft_printf_putchar_fd(input[i], 1);
			i++;
		}
	}
	va_end(args);
	return (total);
}
