/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	stack_alloc(t_ps *ps, int n)
{
	ps->n = n;
	ps->a.v = malloc(sizeof(int) * n);
	ps->b.v = malloc(sizeof(int) * n);
	if (!ps->a.v || !ps->b.v)
		return (0);
	return (1);
}

void	free_ps(t_ps *ps)
{
	free(ps->a.v);
	free(ps->b.v);
	ps->a.v = NULL;
	ps->b.v = NULL;
}

int	is_sorted(t_stack *s)
{
	int	i;

	i = 1;
	while (i < s->size)
	{
		if (s->v[i - 1] > s->v[i])
			return (0);
		i++;
	}
	return (1);
}

int	normalize(t_ps *ps)
{
	int	i;
	int	j;

	i = 0;
	while (i < ps->a.size)
	{
		ps->b.v[i] = 0;
		j = 0;
		while (j < ps->a.size)
		{
			if (j != i && ps->a.v[j] == ps->a.v[i])
				return (0);
			if (ps->a.v[j] < ps->a.v[i])
				ps->b.v[i]++;
			j++;
		}
		i++;
	}
	i = -1;
	while (++i < ps->a.size)
		ps->a.v[i] = ps->b.v[i];
	return (1);
}

void	compute_disorder(t_ps *ps)
{
	int	i;
	int	j;

	i = 0;
	while (i < ps->a.size)
	{
		j = i + 1;
		while (j < ps->a.size)
		{
			ps->pairs++;
			if (ps->a.v[i] > ps->a.v[j])
				ps->mistakes++;
			j++;
		}
		i++;
	}
}
