/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	find_min_pos(t_stack *s)
{
	int	i;
	int	pos;

	pos = 0;
	i = 1;
	while (i < s->size)
	{
		if (s->v[i] < s->v[pos])
			pos = i;
		i++;
	}
	return (pos);
}

int	find_max_pos(t_stack *s)
{
	int	i;
	int	pos;

	pos = 0;
	i = 1;
	while (i < s->size)
	{
		if (s->v[i] > s->v[pos])
			pos = i;
		i++;
	}
	return (pos);
}

void	bring_top_a(t_ps *ps, int pos)
{
	if (pos <= ps->a.size / 2)
		do_ops(ps, RA, pos);
	else
		do_ops(ps, RRA, ps->a.size - pos);
}

void	bring_top_b(t_ps *ps, int pos)
{
	if (pos <= ps->b.size / 2)
		do_ops(ps, RB, pos);
	else
		do_ops(ps, RRB, ps->b.size - pos);
}

int	ft_sqrt(int n)
{
	int	r;

	r = 1;
	while ((r + 1) * (r + 1) <= n)
		r++;
	return (r);
}
