/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 09:47:48 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/10 14:22:48 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*result;
	int		len;
	int		i;

	i = -1;
	if (!s)
		return (NULL);
	len = ft_strlen(s);
	result = malloc((sizeof(char) * len) + 1);
	if (!result)
		return (NULL);
	while (s[++i] != '\0')
		result[i] = f(i, s[i]);
	result[i] = '\0';
	return (result);
}
