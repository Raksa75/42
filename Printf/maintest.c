/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   maintest.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 12:23:09 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/12 13:32:24 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdio.h>

int		ft_somme(int count, ...);
void	ft_chaine(const char *types, ...);

int	main(void)
{
	printf("=== ft_somme ===\n");
	printf("somme = %d\n", ft_somme(3, 10, 20, 12));
	printf("somme = %d\n", ft_somme(0));
	printf("================\n");
	printf("=== ft_chaine ===\n");
	ft_chaine("dsc", 42, "bonjour", 'A');
	printf("================\n");
	return (0);
}
