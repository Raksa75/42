/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_simple.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	sort_simple(t_ps *ps)
{
	while (ps->a.size > 3 && !is_sorted(&ps->a))
	{
		bring_top_a(ps, find_min_pos(&ps->a));
		do_op(ps, PB);
	}
	sort_three(ps);
	do_ops(ps, PA, ps->b.size);
}
