/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_hexapoint.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/13 14:44:48 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/13 14:51:25 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_printf_hexapoint(char *c)
{
	char *adresse;
	int	i;

	i = -1;
	adresse = malloc(sizeof(char *) + 1);
	if (!adresse)
		return (0);
	adresse = &c;
	while (adresse[++i] != '\0')
		ft_printf_putchar_fd(adresse[i], 1);
	free(adresse);
	return (i);
}
