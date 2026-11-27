/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack_ops.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	st_swap(t_stack *s)
{
	int	tmp;

	if (s->size < 2)
		return ;
	tmp = s->v[0];
	s->v[0] = s->v[1];
	s->v[1] = tmp;
}

void	st_push(t_stack *from, t_stack *to)
{
	int	i;

	if (from->size == 0)
		return ;
	i = to->size;
	while (i > 0)
	{
		to->v[i] = to->v[i - 1];
		i--;
	}
	to->v[0] = from->v[0];
	to->size++;
	i = 0;
	while (i < from->size - 1)
	{
		from->v[i] = from->v[i + 1];
		i++;
	}
	from->size--;
}

void	st_rotate(t_stack *s)
{
	int	tmp;
	int	i;

	if (s->size < 2)
		return ;
	tmp = s->v[0];
	i = 0;
	while (i < s->size - 1)
	{
		s->v[i] = s->v[i + 1];
		i++;
	}
	s->v[s->size - 1] = tmp;
}

void	st_rrotate(t_stack *s)
{
	int	tmp;
	int	i;

	if (s->size < 2)
		return ;
	tmp = s->v[s->size - 1];
	i = s->size - 1;
	while (i > 0)
	{
		s->v[i] = s->v[i - 1];
		i--;
	}
	s->v[0] = tmp;
}
