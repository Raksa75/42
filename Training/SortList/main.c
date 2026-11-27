/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nzimmerm <nzimmerm@learner.42.tech>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/27 13:38:37 by nzimmerm          #+#    #+#             */
/*   Updated: 2026/11/27 13:42:37 by nzimmerm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"

int	ascending(int a, int b)
{
	return (a <= b);
}

int	descending(int a, int b)
{
	return (a >= b);
}

static void	push_back(t_list **lst, int data)
{
	t_list	*new;
	t_list	*cur;

	new = malloc(sizeof(t_list));
	if (!new)
		return ;
	new->data = data;
	new->next = NULL;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	cur = *lst;
	while (cur->next)
		cur = cur->next;
	cur->next = new;
}

static void	print_list(char *label, t_list *lst)
{
	printf("%-12s:", label);
	while (lst)
	{
		printf(" %d", lst->data);
		lst = lst->next;
	}
	printf("\n");
}

static void	free_list(t_list *lst)
{
	t_list	*next;

	while (lst)
	{
		next = lst->next;
		free(lst);
		lst = next;
	}
}

static void	test(char *name, int *tab, int size, int (*cmp)(int, int))
{
	t_list	*lst;
	int		i;

	lst = NULL;
	i = 0;
	while (i < size)
		push_back(&lst, tab[i++]);
	printf("--- %s ---\n", name);
	print_list("avant", lst);
	sort_list(&lst, cmp);
	print_list("apres", lst);
	free_list(lst);
}

static int	is_sorted(t_list *lst, int (*cmp)(int, int))
{
	while (lst && lst->next)
	{
		if (!cmp(lst->data, lst->next->data))
			return (0);
		lst = lst->next;
	}
	return (1);
}

/* compte combien de fois val apparait dans la liste */
static int	count(t_list *lst, int val)
{
	int	n;

	n = 0;
	while (lst)
	{
		if (lst->data == val)
			n++;
		lst = lst->next;
	}
	return (n);
}

static int	count_all(t_list *lst)
{
	int	n;

	n = 0;
	while (lst)
	{
		n++;
		lst = lst->next;
	}
	return (n);
}

static int	same_values(t_list *lst, int *tab, int size)
{
	int	i;
	int	in_tab;
	int	j;

	i = 0;
	while (i < size)
	{
		in_tab = 0;
		j = 0;
		while (j < size)
			if (tab[j++] == tab[i])
				in_tab++;
		if (count(lst, tab[i]) != in_tab)
			return (0);
		i++;
	}
	return (count_all(lst) == size);
}

static void	random_tests(int n, int max_size, int range)
{
	int		tab[1000];
	t_list	*lst;
	int		size;
	int		i;
	int		fails;
	int		(*cmp)(int, int);

	fails = 0;
	while (n-- > 0)
	{
		size = rand() % (max_size + 1);
		lst = NULL;
		i = 0;
		while (i < size)
		{
			tab[i] = rand() % (2 * range + 1) - range;
			push_back(&lst, tab[i++]);
		}
		cmp = ascending;
		if (rand() % 2)
			cmp = descending;
		sort_list(&lst, cmp);
		if (!is_sorted(lst, cmp) || !same_values(lst, tab, size))
		{
			fails++;
			printf("ECHEC (%s) taille %d :\n", cmp == ascending
				? "croissant" : "decroissant", size);
			printf("%-12s:", "  avant");
			i = 0;
			while (i < size)
				printf(" %d", tab[i++]);
			printf("\n");
			print_list("  obtenu", lst);
		}
		free_list(lst);
	}
	if (fails == 0)
		printf("\nTests aleatoires : tous OK\n");
	else
		printf("\nTests aleatoires : %d echec(s)\n", fails);
}

int	main(void)
{
	srand(time(NULL));
	int	t1[] = {4, 2, 7, 1, 9, 3};
	int	t2[] = {5, 3, 5, 1, 3, 5};
	int	t3[] = {-3, 10, 0, -8, 2};
	int	t4[] = {42};
	int	t5[] = {1, 2, 3, 4, 5};

	test("croissant", t1, 6, ascending);
	test("decroissant", t1, 6, descending);
	test("doublons", t2, 6, ascending);
	test("negatifs", t3, 5, ascending);
	test("1 element", t4, 1, ascending);
	test("deja trie", t5, 5, ascending);
	test("liste vide", NULL, 0, ascending);
	random_tests(10000, 50, 20);
	return (0);
}
