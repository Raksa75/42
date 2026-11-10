/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 17:27:24 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/10 11:00:32 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*result;
	t_list	*lsttemp;
	void	*ftemp;

	result = NULL;
	if (!lst || !del || !f)
		return (NULL);
	while (lst)
	{
		ftemp = f(lst->content);
		lsttemp = ft_lstnew(ftemp);
		if (!lsttemp)
		{
			del(ftemp);
			ft_lstclear(&result, del);
			return (NULL);
		}
		ft_lstadd_back(&result, lsttemp);
		lst = lst->next;
	}
	return (result);
}
