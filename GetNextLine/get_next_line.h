/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/18 13:49:37 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/18 14:05:57 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# include <stdlib.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

char	*get_next_line(int fd);
int		ft_gnl_strchr(char *s, int c);
int		ft_gnl_strlen(char	*s);
char	*ft_gnl_strjoin(char *s1, char *s2);
char	*ft_gnl_get_line(char *reserve);
char	*ft_gnl_keep_rest(char *reserve);

#endif
