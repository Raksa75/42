/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_small.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	sort_three(t_ps *ps)
{
	int	max;

	if (ps->a.size == 2 && ps->a.v[0] > ps->a.v[1])
		do_op(ps, SA);
	if (ps->a.size != 3)
		return ;
	max = find_max_pos(&ps->a);
	if (max == 0)
		do_op(ps, RA);
	else if (max == 1)
		do_op(ps, RRA);
	if (ps->a.v[0] > ps->a.v[1])
		do_op(ps, SA);
}

void	sort_small(t_ps *ps)
{
	while (ps->a.size > 3)
	{
		bring_top_a(ps, find_min_pos(&ps->a));
		do_op(ps, PB);
	}
	sort_three(ps);
	do_ops(ps, PA, ps->b.size);
}
