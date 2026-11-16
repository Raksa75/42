/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putchar_fd.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 10:03:29 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/16 11:24:08 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_putchar_fd(char c, int fd)
{
	if (!c)
	{
		write(fd, '', 1);
		return (0);
	}
	write(fd, &c, 1);
	return (1);
}
