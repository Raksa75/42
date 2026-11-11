/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 16:59:39 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/04 14:11:14 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (!*little)
		return ((char *)big);
	while (i < len && big[i] != '\0')
	{
		while (big[i + j] == little[j] && (i + j) < len && little[j] != '\0')
			j++;
		if (little[j] == '\0')
			return ((char *)&big[i]);
		j = 0;
		i++;
	}
	return (NULL);
}
