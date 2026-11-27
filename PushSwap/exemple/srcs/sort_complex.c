/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_complex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	radix_pass(t_ps *ps, int bit, int n)
{
	int	k;

	k = 0;
	while (k < n)
	{
		if ((ps->a.v[0] >> bit) & 1)
			do_op(ps, RA);
		else
			do_op(ps, PB);
		k++;
	}
	do_ops(ps, PA, ps->b.size);
}

void	sort_radix(t_ps *ps)
{
	int	n;
	int	bits;
	int	bit;

	n = ps->a.size;
	bits = 0;
	while ((n - 1) >> bits)
		bits++;
	bit = 0;
	while (bit < bits && !is_sorted(&ps->a))
	{
		radix_pass(ps, bit, n);
		bit++;
	}
}
