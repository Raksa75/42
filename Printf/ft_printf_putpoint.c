/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_putpoint.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/13 16:18:15 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 17:52:10 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_putpoint(void *point)
{
	if (!point)
		return (ft_printf_putstr_fd("(nil)", 1));
	ft_printf_putchar_fd('0', 1);
	ft_printf_putchar_fd('x', 1);
	return (2 + ft_convertbase((unsigned long)point, 0));
}
