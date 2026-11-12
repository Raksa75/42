/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maintest.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 12:23:09 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/12 12:36:10 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdio.h>

int		ft_somme(int count, ...);
void	ft_chaine(const char *types, ...);

int	main(void)
{
	printf("somme = %d\n", ft_somme(3, 10, 20, 12));
	printf("somme = %d\n", ft_somme(0));
	ft_chaine("dsc", 42, "bonjour", 'A');
	return (0);
}
