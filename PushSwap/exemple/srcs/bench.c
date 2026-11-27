/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bench.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	print_disorder(t_ps *ps)
{
	long	pct;

	pct = 0;
	if (ps->pairs > 0)
		pct = (ps->mistakes * 20000 + ps->pairs) / (2 * ps->pairs);
	put_str(2, "[bench] disorder: ");
	put_nbr(2, pct / 100);
	put_str(2, ".");
	if (pct % 100 < 10)
		put_str(2, "0");
	put_nbr(2, pct % 100);
	put_str(2, "%\n");
}

static void	print_strategy(t_ps *ps)
{
	put_str(2, "[bench] strategy: ");
	if (ps->mode == M_ADAPTIVE)
		put_str(2, "Adaptive");
	else if (ps->mode == M_SIMPLE)
		put_str(2, "Simple");
	else if (ps->mode == M_MEDIUM)
		put_str(2, "Medium");
	else
		put_str(2, "Complex");
	if (ps->used == M_SIMPLE)
		put_str(2, " / O(n²)\n");
	else if (ps->used == M_MEDIUM)
		put_str(2, " / O(n√n)\n");
	else
		put_str(2, " / O(n log n)\n");
}

static void	print_counts(t_ps *ps, int from, int to)
{
	const char	*name;
	int			len;

	put_str(2, "[bench]");
	while (from <= to)
	{
		name = op_name(from, &len);
		if (write(2, " ", 1) < 0 || write(2, name, len) < 0)
			return ;
		put_str(2, ": ");
		put_nbr(2, ps->count[from]);
		from++;
	}
	put_str(2, "\n");
}

void	print_bench(t_ps *ps)
{
	long	total;
	int		i;

	total = 0;
	i = 0;
	while (i < 11)
		total += ps->count[i++];
	print_disorder(ps);
	print_strategy(ps);
	put_str(2, "[bench] total_ops: ");
	put_nbr(2, total);
	put_str(2, "\n");
	print_counts(ps, SA, PB);
	print_counts(ps, RA, RRR);
}
