/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_chaine.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 12:21:05 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/12 12:36:34 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <stdio.h>

void	ft_chaine(const char *types, ...)
{
	va_list	args; //declaration d'args
	int		i;

	va_start(args, types); //initialisation d'args avec x types
	i = 0;
	while (types[i]) //parcours du nombre de type
	{
		if (types[i] == 'd') //si type d alors
			printf("int : %d\n", va_arg(args, int));
		else if (types[i] == 's') //si type s alors
			printf("chaine : %s\n", va_arg(args, char *));
		else if (types[i] == 'c') //si type c alors
			printf("char : %c\n", va_arg(args, int));
		i++;
	}
	va_end(args); //cloture
}
