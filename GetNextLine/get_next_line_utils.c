/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/16 15:47:31 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/16 15:58:37 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_gnl_strchr(char *s, int c)
{
	int	i;

	if (!s)
		return (0);
	i = -1;
	while (s[++i] != '\0')
		if (s[i] == c)
			return (1);
	return (0);
}

int	ft_gnl_strlen(char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}

char	*ft_gnl_strjoin(char *s1, char *s2)
{
	int		i;
	int		j;
	char	*result;

	i = -1;
	j = -1;
	result = malloc(sizeof(char) * (ft_gnl_strlen(s1) + ft_gnl_strlen(s2)) + 1);
	if (!result)
		return (NULL);
	if (s1)
	{
		while (s1[++i] != '\0')
			result[++j] = s1[i];
	}
	i = -1;
	while (s2[++i] != '\0')
		result[++j] = s2[i];
	result[++j] = '\0';
	free (s1);
	return (result);
}

char	*ft_gnl_get_line(char *reserve)
{
	char	*result;
	int		i;

	i = 0;
	while (reserve[i] != '\n' && reserve[i] != '\0')
		i++;
	if (reserve[i] == '\n')
		i++;
	result = malloc((sizeof(char) * i) + 1);
	if (!result)
		return (NULL);
	result[i] = '\0';
	while (--i >= 0)
		result[i] = reserve[i];
	return (result);
}

char	*ft_gnl_keep_rest(char *reserve)
{
	char	*result;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (ft_gnl_strchr(reserve, '\n') == 0)
	{
		free(reserve);
		return (NULL);
	}
	while (reserve[i] != '\n')
		i++;
	result = malloc(sizeof(char) * (ft_gnl_strlen(reserve) - i));
	if (!result)
	{
		free(reserve);
		return (NULL);
	}
	while (reserve[++i] != '\0')
		result[j++] = reserve[i];
	result[j] = '\0';
	free(reserve);
	return (result);
}
