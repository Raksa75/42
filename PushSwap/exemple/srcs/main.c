/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	init_ps(t_ps *ps)
{
	int	i;

	ps->a.v = NULL;
	ps->a.size = 0;
	ps->b.v = NULL;
	ps->b.size = 0;
	ps->n = 0;
	ps->mode = M_ADAPTIVE;
	ps->mode_set = 0;
	ps->used = M_ADAPTIVE;
	ps->bench = 0;
	ps->mistakes = 0;
	ps->pairs = 0;
	ps->out_len = 0;
	i = 0;
	while (i < 11)
		ps->count[i++] = 0;
}

static int	load(t_ps *ps, int ac, char **av)
{
	int	n;

	if (!parse_args(ps, ac, av))
		return (0);
	n = count_numbers(ac, av);
	if (n < 0)
		return (0);
	if (n == 0)
		return (1);
	if (!stack_alloc(ps, n))
		return (0);
	if (!fill_stack(ps, ac, av))
		return (0);
	return (normalize(ps));
}

static void	run(t_ps *ps)
{
	choose_strategy(ps);
	if (is_sorted(&ps->a))
		return ;
	if (ps->a.size <= 5)
		sort_small(ps);
	else if (ps->used == M_SIMPLE)
		sort_simple(ps);
	else if (ps->used == M_MEDIUM)
		sort_medium(ps);
	else
		sort_radix(ps);
}

int	main(int ac, char **av)
{
	t_ps	ps;

	init_ps(&ps);
	if (!load(&ps, ac, av))
	{
		free_ps(&ps);
		write(2, "Error\n", 6);
		return (1);
	}
	if (ps.n == 0)
		return (0);
	compute_disorder(&ps);
	run(&ps);
	out_flush(&ps);
	if (ps.bench)
		print_bench(&ps);
	free_ps(&ps);
	return (0);
}
