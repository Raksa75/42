/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_numbers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	is_space(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static char	*read_sign(char *s, int *sign)
{
	*sign = 1;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			*sign = -1;
		s++;
	}
	return (s);
}

static int	read_number(char **str, int *out)
{
	char	*s;
	long	nb;
	int		sign;

	s = read_sign(*str, &sign);
	nb = 0;
	if (*s < '0' || *s > '9')
		return (0);
	while (*s >= '0' && *s <= '9')
	{
		nb = nb * 10 + (*s - '0');
		if (nb > 2147483648L)
			return (0);
		s++;
	}
	if (*s && !is_space(*s))
		return (0);
	if (nb * sign > 2147483647L)
		return (0);
	*out = (int)(nb * sign);
	*str = s;
	return (1);
}

static int	read_arg(t_ps *ps, char *s)
{
	while (*s)
	{
		while (is_space(*s))
			s++;
		if (!*s)
			break ;
		if (!read_number(&s, &ps->a.v[ps->a.size]))
			return (0);
		ps->a.size++;
	}
	return (1);
}

int	fill_stack(t_ps *ps, int ac, char **av)
{
	int	i;

	i = 1;
	while (i < ac)
	{
		if (!(av[i][0] == '-' && av[i][1] == '-'))
		{
			if (!read_arg(ps, av[i]))
				return (0);
		}
		i++;
	}
	return (1);
}
