/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pfc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:04:37 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/20 14:44:05 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pfc.h"

int main(void)
{
	ssize_t bytes_read;
	char 	buffer[BUFFER_SIZE];
	char	choixbot;
	char	**choixgeneral;

	*choixgeneral = malloc(sizeof(char *) * 3 + 1);
	choixgeneral[0] = "Pierre";
	choixgeneral[1] = "Papier";
	choixgeneral[2] = "Ciseaux";
	choixgeneral[3] = NULL;
	srand((unsigned int)time(NULL));
	choixbot = ((rand() % 3) + 1) + '0';

	ft_printf("Pierre ? [1]\nPapier ? [2]\nCiseaux ? [3]\n");
	bytes_read = read(0, buffer, BUFFER_SIZE-1);
	if (bytes_read == -1)
	{
		perror("Erreur sur le read");
		return (1);
	}
	buffer[bytes_read] = '\0';
	if (buffer[0] != '1' || buffer[0] != '2' || buffer[0] != 3)
	{
		ft_printf("Veillez saisir un caractere valide.");
		return (1);
	}
	if (ft_strlen(buffer) > 1)
	{
		ft_printf("Veillez saisir un caractere valide.");
		return (1);
	}
	ft_printf("Vous avez choisis %s.\nBot choisis %s.\n", choixgeneral[buffer[0] - '0' + 1], choixgeneral[choixbot + 1]);
	if (buffer[0] == choixbot)
		ft_printf("Match nul.\n");
	if (choixbot + 1 == buffer[0] || (choixbot == '3' && buffer[0] == '1')
		ft_printf("Vous avez gagner !\n");
	else
		ft_printf("Vous avez perdu.\n");
	return (0);
