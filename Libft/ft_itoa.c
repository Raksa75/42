/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/06 16:06:12 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/12 11:50:33 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_zero(void)
{
	char	*result;

	result = malloc(sizeof(char) * 2);
	if (!result)
		return (NULL);
	result[0] = '0';
	result[1] = '\0';
	return (result);
}

static int	ft_itoasize(long ncopy)
{
	int	i;

	i = 0;
	if (ncopy < 0)
	{
		ncopy = ncopy * -1;
		i++;
	}
	while (ncopy != 0)
	{
		ncopy = ncopy / 10;
		i++;
	}
	return (i);
}

static char	*ft_fillitoa(long ncopy, char *result, int i)
{
	if (ncopy < 0)
	{
		ncopy = ncopy * -1;
		result[0] = '-';
	}
	while (ncopy != 0)
	{
		result[i - 1] = ncopy % 10 + '0';
		ncopy = ncopy / 10;
		i--;
	}
	return (result);
}

char	*ft_itoa(int n)
{
	char	*result;
	long	i;
	long	ncopy;

	ncopy = n;
	i = (long)ft_itoasize(ncopy);
	if (n == 0)
	{
		result = ft_zero();
		return (result);
	}
	result = malloc((sizeof(char) * i) + 1);
	if (!result)
		return (NULL);
	result[i] = '\0';
	result = ft_fillitoa(ncopy, result, i);
	return (result);
}
