/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	push_chunks(t_ps *ps, int width)
{
	int	pushed;

	pushed = 0;
	while (ps->a.size > 0)
	{
		if (ps->a.v[0] <= pushed)
		{
			do_op(ps, PB);
			do_op(ps, RB);
			pushed++;
		}
		else if (ps->a.v[0] <= pushed + width)
		{
			do_op(ps, PB);
			pushed++;
		}
		else
			do_op(ps, RA);
	}
}

static void	pull_back(t_ps *ps)
{
	while (ps->b.size > 0)
	{
		bring_top_b(ps, find_max_pos(&ps->b));
		do_op(ps, PA);
	}
}

void	sort_medium(t_ps *ps)
{
	int	width;

	width = ft_sqrt(ps->a.size) * 3 / 2;
	if (width < 1)
		width = 1;
	push_chunks(ps, width);
	pull_back(ps);
}
