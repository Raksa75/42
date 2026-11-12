/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_somme.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 12:12:04 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/12 13:30:13 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <stdio.h>

int	ft_somme(int count, ...)
{
	va_list args; //type va_list args
	int	total;
	int	i;

	va_start(args, count); //commence a args, contient count arguments
	total = 0;
	i = 0;
	while (i < count)
	{
		total = total + va_arg(args, int); //regarde args, de type int et avance le curseur
		i++;
	}
	va_end(args); //cloture
	return (total);
}
