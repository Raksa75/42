/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 14:04:08 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 17:44:57 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "libft.h"
# include "stdarg.h"
# include "unistd.h"

int	ft_printf(const char *input, ...);
int	ft_printf_putchar_fd(char c, int fd);
int	ft_printf_putstr_fd(char *s, int fd);
int	ft_printf_putnbr_fd(long n, int fd);
int	ft_printf_putpoint(void *point);
int	ft_convertbase(unsigned long i, int option);

#endif
