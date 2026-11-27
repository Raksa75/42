/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   adaptive.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	choose_strategy(t_ps *ps)
{
	if (ps->mode != M_ADAPTIVE)
	{
		ps->used = ps->mode;
		return ;
	}
	if (ps->mistakes * 5 < ps->pairs)
		ps->used = M_SIMPLE;
	else if (ps->mistakes * 2 < ps->pairs)
		ps->used = M_MEDIUM;
	else
		ps->used = M_COMPLEX;
}
