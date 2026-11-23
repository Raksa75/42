/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pfc.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 14:04:37 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/23 14:58:09 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pfc.h"

static void	ft_clear(void)
{
	ft_printf("\033[2J\033[H");
}

static void	ft_pause(void)
{
	ssize_t	bytes_read;
	char	buffer[BUFFER_SIZE];

	while (buffer[0] != '\n')
	{
		ft_printf("\nAppuie sur Entree pour continuer");
		bytes_read = read(0, buffer, BUFFER_SIZE - 1);
		buffer[bytes_read] = '\0';
	}
	ft_clear();
}

static void	ft_score(int *score)
{
	ft_printf("Vous avez actuelement :\n%d victoires\n%d defaites\n%d egalite\n", score[0], score[1], score[2]);
	ft_pause();
	ft_clear();
	return ;
}

static void	ft_jouer(int *score)
{
	ssize_t	bytes_read;
	char	buffer[BUFFER_SIZE];
	char	*choixgeneral[4];
	int		choixbot;
	int		joueur;

	choixgeneral[0] = "Pierre";
	choixgeneral[1] = "Papier";
	choixgeneral[2] = "Ciseaux";
	choixgeneral[3] = NULL;
	choixbot = (rand() % 3);
	ft_printf("Pierre ? [1]\nPapier ? [2]\nCiseaux ? [3]\n");
	bytes_read = read(0, buffer, BUFFER_SIZE - 1);
	if (bytes_read == -1)
	{
		perror("Erreur sur le read");
		ft_pause();
		ft_clear();
		return ;
	}
	buffer[bytes_read] = '\0';
	while ((bytes_read != 0) &&((buffer[0] != '1' && buffer[0] != '2' && buffer[0] != '3') || (buffer[1] != '\0' && buffer[1] != '\n')))
	{
		ft_printf("Veuillez saisir un caractere valide.\n");
		bytes_read = read(0, buffer, BUFFER_SIZE - 1);
		buffer[bytes_read] = '\0';
	}
	ft_clear();
	if (bytes_read <= 0)
		return ;
	joueur = buffer[0] - '0' - 1;
	ft_printf("Vous avez choisi %s.\nLe bot choisi %s.\n", choixgeneral[joueur], choixgeneral[choixbot]);
	ft_pause();
	if (joueur == choixbot)
	{
		score[2]++;
		ft_score(score);
		ft_printf("Match nul.\n");
		ft_pause();
		ft_clear();
	}
	else if ((joueur == choixbot + 1) || (joueur == 0 && choixbot == 2))
	{
		score[0]++;
		ft_score(score);
		ft_printf("Vous avez gagne !\n");
		ft_pause();
		ft_clear();
	}
	else
	{
		score[1]++;
		ft_score(score);
		ft_printf("Vous avez perdu.\n");
		ft_pause();
		ft_clear();
	}
	return ;
}

static int	ft_choix(int *score)
{
	ssize_t	bytes_read;
	char	buffer[BUFFER_SIZE];

	ft_clear();
	ft_printf("Jouer ! [1]\nMon score ! [2]\nQuitter. [3]\n");
	bytes_read = read(0, buffer, BUFFER_SIZE - 1);
	buffer[bytes_read] = '\0';
	while (bytes_read != 0 && ((buffer[0] != '1' && buffer[0] != '2' && buffer[0] != '3') || (buffer[1] != '\0' && buffer[1] != '\n')))
	{
		ft_printf("Veuillez saisir un caractere valide.\n");
		bytes_read = read(0, buffer, BUFFER_SIZE - 1);
		buffer[bytes_read] = '\0';
	}
	if (buffer[0] == '1')
	{
		ft_clear();
		ft_jouer(score);
		return (1);
	}
	else if (buffer[0] == '2')
	{
		ft_clear();
		ft_score(score);
		return (1);
	}
	else
	{
		ft_clear();
		ft_printf("A+ !\n");
		return (0);
	}
}

int	main(void)
{
	int	i;
	int	score[3];

	srand((unsigned int)time(NULL));
	score[0] = 0;
	score[1] = 0;
	score[2] = 0;
	i = 1;
	while (i == 1)
		i = ft_choix(score);
	return (0);
}
