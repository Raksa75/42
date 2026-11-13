/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 14:02:38 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 17:51:23 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_conv(char c, va_list *args)
{
	if (c == 'c')
		return (ft_printf_putchar_fd(va_arg(*args, int), 1));
	else if (c == 's')
		return (ft_printf_putstr_fd(va_arg(*args, char *), 1));
	else if (c == 'p')
		return (ft_printf_putpoint(va_arg(*args, char *)));
	else if (c == 'd' || input[i + 1] == 'i')
		return (ft_printf_putnbr_fd(va_arg(*args, int), 1));
	else if (c == 'u')
		return (ft_printf_putnbr_fd(va_arg(*args, unsigned int), 1));
	else if (c == 'x')
		return (ft_convertbase(va_arg(*args, unsigned int), 0));
	else if (c == 'X')
		return (ft_convertbase(va_arg(*args, unsigned int), 1));
	else if (c == '%')
		return (ft_printf_putchar_fd('%', 1));
	else
		return (ft_printf_putchar_fd('%', 1));
}

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
			total += ft_conv(input[i + 1], &args);
			i += 2;
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
