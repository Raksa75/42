/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 13:31:31 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 13:35:49 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIST_H
# define LIST_H

typedef struct s_list
{
	int				data;
	struct s_list	*next;
}	t_list;

void	sort_list(t_list **lst, int (*f)(int a, int b));

#endif
