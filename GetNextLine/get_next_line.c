/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/16 12:17:20 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/16 15:57:18 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	char		*ligne;
	static char	*reserve;
	char		buf[BUFFER_SIZE + 1];
	int			n;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	while (ft_gnl_strchr(reserve, '\n') == 0)
	{
		n = read(fd, buf, BUFFER_SIZE);
		if (n == -1)
		{
			free(reserve);
			reserve = NULL;
			return (NULL);
		}
		if (n == 0)
			break ;
		else
		{
			buf[n] = '\0';
			reserve = ft_gnl_strjoin(reserve, buf);
		}
	}
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
