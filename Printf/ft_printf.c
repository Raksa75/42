/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 14:02:38 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/12 15:24:40 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

int	ft_printf(const char *input, ...)
{
	va_list	args; //declaration de args
	int	i;
	int	total;

	va_start(args, input);
	total = 0;
	i = 0;
	while (input[i] != '\0')
	{
		if (type[i] == '%')
		{
			if (type[i + 1] == 'c')
				total = total + ft_printf_putchar_fd(va_arg(args, int), 1);
			else if (type[i + 1] == 's')
				total += ft_printf_putstr_fd(va_arg(args, char *), 1);
			else if (type[i + 1] == 'p')
				total == 
			else if (type[i + 1] == 'd' || type[i] == 'i')
				total += ft_printf_putnbr(va_arg(args, int), 1);
			else if (type[i + 1] == 'u')
				total +=
			else if (type[i + 1] == 'x')
				total +=
			else if (type[i + 1] == 'X')
				total +=
			else if (type[i + 1] == '%')
				total += ft_printf_putchar_fd('%', 1);
			else
				total += ft_printf_putchar_fd('%', 1);
			i++;
		}
		else
			total += ft_printf_putchar_fd(type[i], 1);
	}
	va_end(args);
	return (total);
}
