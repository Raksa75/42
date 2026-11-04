/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 14:41:38 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/04 15:28:15 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	strlendest;
	size_t	strlensrc;
	size_t	j;
	size_t	i;

	strlendest = ft_strlen(dest);
	strlensrc = ft_strlen(src);
	j = 0;
	i = strlendest;
	if (size <= strlendest)
		return (strlensrc + size);
	while (src[j] != '\0' && strlendest + 1 < size)
	{
		dest[strlendest] = src[j];
		strlendest++;
		j++;
	}
	dest[strlendest] = '\0';
	return (strlensrc + i);
}
