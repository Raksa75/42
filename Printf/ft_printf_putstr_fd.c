/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putstr_fd.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 09:40:25 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 17:51:48 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_putstr_fd(char *s, int fd)
{
	int	i;

	if (!s)
	{
		ft_printf_putstr_fd("(null)", fd);
		return (6);
	}
	i = -1;
	while (s[++i] != '\0')
		ft_printf_putchar_fd(s[i], fd);
	return (i);
}
