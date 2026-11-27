/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 12:37:45 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 13:30:22 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "list.h"

static void	list_swap(t_list *lsta, t_list *lstb)
{
	int	temp;

	temp = 0;
	temp = lsta->data;
	lsta->data = lstb->data;
	lstb->data = temp;
}

void	sort_list(t_list **lst, int (*f)(int a, int b))
{
	t_list	*temp;

	if (!f || !lst)
		return ;
	temp = *lst;
	while (temp && temp->next)
	{
		if (f(temp->data, temp->next->data) == 0)
		{
			list_swap(temp, temp->next);
			temp = *lst;
		}

		else
			temp = temp->next;
	}
}
