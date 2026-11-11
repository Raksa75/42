/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 16:19:58 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/04 16:58:17 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_isinset(char c, const char *set)
{
	size_t	i;

	i = 0;
	while (set[i] != '\0')
	{
		if (set[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*result;
	int		i;
	int		start;
	int		end;

	i = 0;
	start = 0;
	end = 0;
	if (!s1 || !set)
		return (NULL);
	while (s1[start] && ft_isinset(s1[start], set))
		start++;
	end = ft_strlen(s1);
	if (end == 0 || start == end)
		return (ft_strdup(""));
	end--;
	while (end > start && ft_isinset(s1[end], set))
		end--;
	result = malloc(sizeof(char) * (end - start + 2));
	if (!result)
		return (NULL);
	while (start <= end)
		result[i++] = s1[start++];
	result[i] = '\0';
	return (result);
}
