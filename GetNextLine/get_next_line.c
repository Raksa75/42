/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/16 12:17:20 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/18 13:45:50 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_gnl_read(int fd, char *reserve)
{
	char	buf[BUFFER_SIZE + 1];
	int		n;

	while (!ft_gnl_strchr(reserve, '\n'))
	{
		n = read(fd, buf, BUFFER_SIZE);
		if (n == -1)
		{
			free(reserve);
			return (NULL);
		}
		if (n == 0)
			break ;
		buf[n] = '\0';
		reserve = ft_gnl_strjoin(reserve, buf);
		if (!reserve)
			return (NULL);
	}
	return (reserve);
}

char	*get_next_line(int fd)
{
	char		*ligne;
	static char	*reserve;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	reserve = ft_gn_read(fd, reserve);
	if (!reserve || reserve[0] == '\0')
	{
		free(reserve);
		reserve = NULL;
		return (NULL);
	}
	ligne = ft_gnl_get_line(reserve);
	reserve = ft_gnl_keep_reste(reserve);
	return (ligne);
}
