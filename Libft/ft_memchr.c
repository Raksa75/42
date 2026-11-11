/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 16:30:14 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/04 15:41:36 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*resultat;
	size_t				i;

	i = 0;
	resultat = s;
	while (i < n)
	{
		if (resultat[i] == (unsigned char)c)
			return ((void *)&resultat[i]);
		i++;
	}
	return (NULL);
}
