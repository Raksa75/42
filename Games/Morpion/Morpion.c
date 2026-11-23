/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Morpion.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:27:41 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/23 15:55:04 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void	ft_ia_moove(void)
{

}

void	ft_wincheck(void)
{

}

void	ft_canplace(void)
{

}

static void	ft_jouer(int *score)
{
	ssize_t	bytes_read;
	char	buffer[BUFFER_SIZE];
	char	*tab[];


}

int	main(void)
{
	int	i;
	int score[3];

	srand((unsigned int)time(NULL));
	score[0] = 0;
	score[1] = 0;
	score[2] = 0;
	i = 1;
	ft_printf("Bienvenue dans le jeu du morpion. Pour placer un element, appuiyez sur le chiffre corespondant de votre pave numerique.\n");
	ft_pause();
	ft_clear();
	while (i == 1)
		i = ft_choix(score);
	return (0);
}
