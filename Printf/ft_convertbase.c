/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_convertbase.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/13 15:26:23 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 16:58:47 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

static char	*ft_size(unsigned long i, int *j)
{
	*j = 1;
	while (i >= 16)
	{
		i /= 16;
		(*j)++;
	}
	return (malloc(sizeof(char) * (*j + 1)));
}

static int	ft_convert(unsigned long i, int option)
{
	char			*base;
	char			*result;
	int				j;

	if (option == 0)
		base = "0123456789abcdef";
	else
		base = "0123456789ABCDEF";
	result = ft_size(i, &j);
	if (!result)
		return (0);
	result[j] = '\0';
	while (i >= 16)
	{
		result[--j] = base[i % 16];
		i /= 16;
	}
	result[--j] = base[i % 16];
	i = ft_printf_putstr_fd(result, 1);
	free(result);
	return (i);
}

int	ft_convertbase(unsigned long i, int option)
{
	if (option < 0 || option > 1)
		return (ft_printf_putstr_fd("Mauvaise utilisation", 1));
	else
		return (ft_convert(i, option));
}
