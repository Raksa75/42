/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 14:02:38 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 17:22:03 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

static int	ft_conv(char c, va_list *args)
{
	if (input[i + 1] == 'c')
		return (ft_printf_putchar_fd(va_arg(args, int), 1));
	else if (input[i + 1] == 's')
		return (ft_printf_putstr_fd(va_arg(args, char *), 1));
	else if (input[i + 1] == 'p')
		return (ft_printf_putpoint(va_arg(args, char *)));
	else if (input[i + 1] == 'd' || input[i + 1] == 'i')
		return (ft_printf_putnbr_fd(va_arg(args, int), 1));
	else if (input[i + 1] == 'u')
		return (ft_printf_putnbr_fd(va_arg(args, unsigned int), 1));
	else if (input[i + 1] == 'x')
		return (ft_convertbase(va_arg(args, unsigned int), 0));
	else if (input[i + 1] == 'X')
		return (ft_convertbase(va_arg(args, unsigned int), 1));
	else if (input[i + 1] == '%')
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
