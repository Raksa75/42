#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int	ascending(int a, int b)
{
	return (a <= b);
}

int	descending(int a, int b)
{
	return (a >= b);
}

/* ajoute un element a la fin de la liste */
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

/* construit une liste a partir d'un tableau, la trie et l'affiche */
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

int	main(void)
{
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
	return (0);
}
