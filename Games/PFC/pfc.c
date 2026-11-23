/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pfc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:04:37 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/23 10:54:44 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pfc.h"

int main(void)
{
	ssize_t	bytes_read;
	char 	buffer[BUFFER_SIZE];
	char	*choixgeneral[4];
	int		choixbot;
	int 	joueur;

	choixgeneral[0] = "Pierre";
	choixgeneral[1] = "Papier";
	choixgeneral[2] = "Ciseaux";
	choixgeneral[3] = NULL;
	srand((unsigned int)time(NULL));
	choixbot = (rand() % 3);

	ft_printf("Pierre ? [1]\nPapier ? [2]\nCiseaux ? [3]\n");
	bytes_read = read(0, buffer, BUFFER_SIZE-1);
	if (bytes_read == -1)
	{
		perror("Erreur sur le read");
		return (1);
	}
	buffer[bytes_read] = '\0';
	joueur = buffer[0] - '0' - 1;
	if (buffer[0] != '1' && buffer[0] != '2' && buffer[0] != '3')
	{
		ft_printf("Veuillez saisir un caractere valide.\n");
		return (1);
	}
	if (buffer[1] != '\0' && buffer[1] != '\n')
	{
		ft_printf("Veuillez saisir un caractere valide.\n");
		return (1);
	}
	ft_printf("Vous avez choisi %s.\nLe bot choisi %s.\n", choixgeneral[joueur], choixgeneral[choixbot]);
	if (joueur == choixbot)
		ft_printf("Match nul.\n");
	else if ((joueur == choixbot + 1) || (joueur == 0 && choixbot == 2))
		ft_printf("Vous avez gagne !\n");
	else
		ft_printf("Vous avez perdu.\n");
	return (0);
}
