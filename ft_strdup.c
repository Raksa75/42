/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 14:00:45 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/04 15:42:02 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s1)
{
	char	*s2;
	int		s1len;
	int		i;

	i = 0;
	if (!s1)
		return (NULL);
	s1len = ft_strlen(s1);
	s2 = malloc(sizeof(char) * (s1len + 1));
	if (!s2)
		return (NULL);
	while (i < s1len)
	{
		s2[i] = s1[i];
		i++;
	}
	s2[i] = '\0';
	return (s2);
}
