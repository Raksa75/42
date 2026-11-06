/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/04 16:58:46 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/06 16:03:58 by nzimmerm         ###   ########.fr       */
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

char	**ft_malloccheck(char **result, char const *s, char c)
{
	if (!s)
		return (NULL);
	result = malloc(sizeof(char *) * (ft_count_words(s, c) + 1));
	if (!result)
		return (NULL);
	return (result);
}

char	**ft_split(char const *s, char c)
{
	char	**result;

	int (wordlen) = 0;
	int (i) = 0;
	int (j) = 0;
	result = ft_malloccheck(result, s, c);
	if (result == NULL)
		return (NULL);
	while (s[i] != '\0')
	{
		while (s[i] != '\0' && s[i] == c)
			i++;
		if (s[i] != '\0')
		{
			while (s[i + wordlen] != '\0' && s[i + wordlen] != c)
				wordlen++;
			result[j] = ft_substr(s, i, wordlen);
			if (!result[j])
				return (NULL);
			j++;
			i = i + wordlen;
		}
	}
	result[j] = NULL;
	return (result);
}
