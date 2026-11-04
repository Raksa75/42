/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 15:55:05 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/04 15:40:19 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int		i;
	int		slen;
	char	*ref;

	i = -1;
	ref = NULL;
	slen = ft_strlen(s);
	while (++i <= slen)
	{
		if (s[i] == (char)c)
			ref = (char *)&s[i];
	}
	return (ref);
}
