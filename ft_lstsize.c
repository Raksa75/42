/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 15:47:14 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/09 16:50:33 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

unsigned int	ft_lstsize(t_list *lst)
{
	unsigned int	i;
	t_list			*temp;

	i = 1;
	temp = lst;
	if (!lst)
		return (0);
	while (temp->next != NULL)
	{
		i++;
		temp = temp->next;
	}
	return (i);
}
