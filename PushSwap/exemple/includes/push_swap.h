/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 15:20:00 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 15:20:00 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include <unistd.h>
# include <stdlib.h>

# define OUT_SIZE 4096

typedef enum e_op
{
	SA,
	SB,
	SS,
	PA,
	PB,
	RA,
	RB,
	RR,
	RRA,
	RRB,
	RRR
}	t_op;

typedef enum e_mode
{
	M_ADAPTIVE,
	M_SIMPLE,
	M_MEDIUM,
	M_COMPLEX
}	t_mode;

typedef struct s_stack
{
	int	*v;
	int	size;
}	t_stack;

typedef struct s_ps
{
	t_stack	a;
	t_stack	b;
	int		n;
	t_mode	mode;
	int		mode_set;
	t_mode	used;
	int		bench;
	long	count[11];
	long	mistakes;
	long	pairs;
	char	out[OUT_SIZE];
	int		out_len;
}	t_ps;

int			parse_args(t_ps *ps, int ac, char **av);
int			count_numbers(int ac, char **av);
int			fill_stack(t_ps *ps, int ac, char **av);

int			stack_alloc(t_ps *ps, int n);
void		free_ps(t_ps *ps);
int			is_sorted(t_stack *s);
int			normalize(t_ps *ps);
void		compute_disorder(t_ps *ps);

void		st_swap(t_stack *s);
void		st_push(t_stack *from, t_stack *to);
void		st_rotate(t_stack *s);
void		st_rrotate(t_stack *s);

void		do_op(t_ps *ps, t_op op);
void		do_ops(t_ps *ps, t_op op, int times);
void		out_add(t_ps *ps, const char *s, int len);
void		out_flush(t_ps *ps);
const char	*op_name(t_op op, int *len);

int			find_min_pos(t_stack *s);
int			find_max_pos(t_stack *s);
void		bring_top_a(t_ps *ps, int pos);
void		bring_top_b(t_ps *ps, int pos);
int			ft_sqrt(int n);

int			ft_strcmp(const char *a, const char *b);
void		put_str(int fd, const char *s);
void		put_nbr(int fd, long n);

void		sort_three(t_ps *ps);
void		sort_small(t_ps *ps);
void		sort_simple(t_ps *ps);
void		sort_medium(t_ps *ps);
void		sort_radix(t_ps *ps);
void		choose_strategy(t_ps *ps);
void		print_bench(t_ps *ps);

#endif
