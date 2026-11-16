/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/16 12:17:20 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/16 14:30:06 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int ft_gnl_strchr(char *s, int c)
{
	int	i;

	i = -1
	while (s[++i] != '\0')
		if (s[i] == '\n')
			return (1);
	return (0);
}

int	ft_gnl_strlen(char	*s)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}
char	*ft_gnl_strjoin(char *s1, char *s2)
{
	int	i;
	int	j;
	char	*result;

	if (!s1 || !s2)
		return (NULL);
	i = -1;
	j = -1;
	result = malloc(sizeof(char) * ft_gnl_strlen(s1) + ft_gnl_strlen(s2) + 1);
	while (s1[++i] != '\0')
		result[++j] = s1[i];
	i = -1;
	while (s2[++i] != '\0')
		result[++j] = s2[i];
	result[++j] = '\0';
	return (result);
}

char	*get_next_line(int fd)
{
	static char	*reserve;
	char	buf[BUFFER_SIZE + 1];
	int	i;
	int	n;

	i = 0;
	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (ft_gnl_strchr(reserve, '\n') == 0)
	{
		n = read(fd, buf, BIFER_SIZE);
		if (n == -1)
		{
			free(reserve);
			reserve = NULL;
			return (reserve = NULL);
		}
		if (n == 0)
			break;
		else
		{
			buf[n] = '\0';
			reserve = ft_gnl_strjoin(reserve, buf);
		}
	}
	if (!reserve)
	{
		free(reserve);
		return (NULL);
	}
}
