/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 16:58:46 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/09 11:59:13 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_count_words(char const *s, char c)
{
	int	i;
	int	count;
	int	flag;

	i = 0;
	count = 0;
	flag = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c && flag == 0)
		{
			count++;
			flag = 1;
		}
		else if (s[i] == c)
			flag = 0;
		i++;
	}
	return (count);
}

char	**ft_malloccheck(char const *s, char c)
{
	char	**result;

	if (!s)
		return (NULL);
	result = malloc(sizeof(char *) * (ft_count_words(s, c) + 1));
	if (!result)
		return (NULL);
	return (result);
}

void	*ft_free(char **result, int j)
{
	while (j > 0)
	{
		j--;
		free(result[j]);
	}
	free(result);
	return (NULL);
}

char	**ft_split(char const *s, char c)
{
	char	**result;

	int (start) = 0;
	int (i) = 0;
	int (j) = 0;
	result = ft_malloccheck(s, c);
	if (result == NULL)
		return (NULL);
	while (s[i] != '\0')
	{
		while (s[i] != '\0' && s[i] == c)
			i++;
		if (s[i] != '\0')
		{
			start = i;
			while (s[i] != '\0' && s[i] != c)
				i++;
			result[j] = ft_substr(s, start, i - start);
			if (!result[j])
				return (ft_free(result, j));
			j++;
		}
	}
	result[j] = NULL;
	return (result);
}
