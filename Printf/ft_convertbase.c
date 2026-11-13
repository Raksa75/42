/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_convertbase.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/13 15:26:23 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 16:09:16 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "ft_printf.h"

int	ft_convert(unsigned long i, int option)
{
	char			*base;
	char			*result;
	int				j;
	unsigned long	temp;

	if (option == 1)
		base = "0123456789abcdef";
	else
		base = "0123456789ABCDEF";
	j = 1;
	temp = i;
	while (i >= 16)
	{
		i /= 16;
		j++;
	}
	result = malloc((sizeof(char) * j) + 1);
	if (!result)
		return(0);
	result[j] = '\0';
	while (temp >= 16)
	{
		result[--j] = base[temp % 16];
		temp /= 16;
	}
	result[--j] = base[temp % 16];
	i = ft_printf_putstr(result ,1);
	free(result);
	return (i);
}

int	ft_convertbase(unsigned long i, int option)
{
	if (option < 1 || option > 2)
		return (ft_putstr_fd("Mauvaise utilisation de ft_convertbase",1);
	else
		return (ft_convert(i, option));
}
