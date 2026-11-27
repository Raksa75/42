/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	set_mode(t_ps *ps, t_mode mode)
{
	if (ps->mode_set)
		return (0);
	ps->mode = mode;
	ps->mode_set = 1;
	return (1);
}

static int	parse_flag(t_ps *ps, char *s)
{
	if (ft_strcmp(s, "--simple") == 0)
		return (set_mode(ps, M_SIMPLE));
	if (ft_strcmp(s, "--medium") == 0)
		return (set_mode(ps, M_MEDIUM));
	if (ft_strcmp(s, "--complex") == 0)
		return (set_mode(ps, M_COMPLEX));
	if (ft_strcmp(s, "--adaptive") == 0)
		return (set_mode(ps, M_ADAPTIVE));
	if (ft_strcmp(s, "--bench") == 0 && !ps->bench)
	{
		ps->bench = 1;
		return (1);
	}
	return (0);
}

int	parse_args(t_ps *ps, int ac, char **av)
{
	int	i;

	i = 1;
	while (i < ac)
	{
		if (av[i][0] == '-' && av[i][1] == '-')
		{
			if (!parse_flag(ps, av[i]))
				return (0);
		}
		i++;
	}
	return (1);
}

static int	count_words(char *s)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] == ' ' || (s[i] >= 9 && s[i] <= 13))
			i++;
		if (s[i])
			count++;
		while (s[i] && s[i] != ' ' && !(s[i] >= 9 && s[i] <= 13))
			i++;
	}
	return (count);
}

int	count_numbers(int ac, char **av)
{
	int	i;
	int	total;
	int	words;

	i = 1;
	total = 0;
	while (i < ac)
	{
		if (!(av[i][0] == '-' && av[i][1] == '-'))
		{
			words = count_words(av[i]);
			if (words == 0)
				return (-1);
			total += words;
		}
		i++;
	}
	return (total);
}
