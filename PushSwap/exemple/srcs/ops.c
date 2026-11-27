/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ops.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

const char	*op_name(t_op op, int *len)
{
	const char	*names;

	names = "sa sb ss pa pb ra rb rr rrarrbrrr";
	*len = 2;
	if (op >= RRA)
		*len = 3;
	return (names + op * 3);
}

void	out_flush(t_ps *ps)
{
	if (ps->out_len > 0)
	{
		if (write(1, ps->out, ps->out_len) < 0)
			ps->out_len = 0;
	}
	ps->out_len = 0;
}

void	out_add(t_ps *ps, const char *s, int len)
{
	int	i;

	if (ps->out_len + len > OUT_SIZE)
		out_flush(ps);
	i = 0;
	while (i < len)
	{
		ps->out[ps->out_len++] = s[i];
		i++;
	}
}

void	do_op(t_ps *ps, t_op op)
{
	const char	*name;
	int			len;

	if (op == SA || op == SS)
		st_swap(&ps->a);
	if (op == SB || op == SS)
		st_swap(&ps->b);
	if (op == PA)
		st_push(&ps->b, &ps->a);
	if (op == PB)
		st_push(&ps->a, &ps->b);
	if (op == RA || op == RR)
		st_rotate(&ps->a);
	if (op == RB || op == RR)
		st_rotate(&ps->b);
	if (op == RRA || op == RRR)
		st_rrotate(&ps->a);
	if (op == RRB || op == RRR)
		st_rrotate(&ps->b);
	name = op_name(op, &len);
	out_add(ps, name, len);
	out_add(ps, "\n", 1);
	ps->count[op]++;
}

void	do_ops(t_ps *ps, t_op op, int times)
{
	while (times > 0)
	{
		do_op(ps, op);
		times--;
	}
}
