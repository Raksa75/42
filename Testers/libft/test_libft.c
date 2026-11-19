/*
** test_libft.c - testeur de la Libft
** Lance-le avec ./run.sh (il compile la libft et ce fichier).
*/

#include "../common/tester.h"
#include "libft.h"
#include <ctype.h>
#include <limits.h>
#include <stdint.h>

/* ---------- references pour les fonctions absentes de la libc ---------- */

static size_t	ref_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	len;

	len = strlen(src);
	if (size > 0)
	{
		if (len >= size)
		{
			memcpy(dst, src, size - 1);
			dst[size - 1] = '\0';
		}
		else
			memcpy(dst, src, len + 1);
	}
	return (len);
}

static size_t	ref_strlcat(char *dst, const char *src, size_t size)
{
	size_t	dlen;
	size_t	slen;
	size_t	i;

	dlen = strnlen(dst, size);
	slen = strlen(src);
	if (dlen == size)
		return (size + slen);
	i = 0;
	while (src[i] && dlen + i < size - 1)
	{
		dst[dlen + i] = src[i];
		i++;
	}
	dst[dlen + i] = '\0';
	return (dlen + slen);
}

static char	*ref_strnstr(const char *big, const char *little, size_t len)
{
	size_t	l;
	size_t	i;

	l = strlen(little);
	if (l == 0)
		return ((char *)big);
	i = 0;
	while (big[i] && i + l <= len)
	{
		if (strncmp(big + i, little, l) == 0)
			return ((char *)big + i);
		i++;
	}
	return (NULL);
}

static int	sign(int n)
{
	return ((n > 0) - (n < 0));
}

/* lit tout ce qui a ete ecrit dans un pipe */
static size_t	read_pipe(int fd, char *buf, size_t size)
{
	ssize_t	n;
	size_t	total;

	total = 0;
	n = read(fd, buf, size - 1);
	while (n > 0)
	{
		total += n;
		n = read(fd, buf + total, size - 1 - total);
	}
	buf[total] = '\0';
	return (total);
}

/* ============================== PARTIE 1 ============================== */

static void	t_isfuncs(void)
{
	int	c;

	c = -1;
	while (c <= 255)
	{
		CHECK(!!ft_isalpha(c) == !!isalpha(c), "ft_isalpha(%d)", c);
		CHECK(!!ft_isdigit(c) == !!isdigit(c), "ft_isdigit(%d)", c);
		CHECK(!!ft_isalnum(c) == !!isalnum(c), "ft_isalnum(%d)", c);
		CHECK(!!ft_isascii(c) == (c >= 0 && c <= 127), "ft_isascii(%d)", c);
		CHECK(!!ft_isprint(c) == !!isprint(c), "ft_isprint(%d)", c);
		c++;
	}
}

static void	t_toupper_tolower(void)
{
	int	c;

	c = -1;
	while (c <= 255)
	{
		CHECK(ft_toupper(c) == toupper(c), "ft_toupper(%d) = %d, attendu %d",
			c, ft_toupper(c), toupper(c));
		CHECK(ft_tolower(c) == tolower(c), "ft_tolower(%d) = %d, attendu %d",
			c, ft_tolower(c), tolower(c));
		c++;
	}
}

static void	t_strlen(void)
{
	const char	*tests[] = {"", "a", "bonjour", "avec\tdes\nespaces", NULL};
	char		big[10001];
	int			i;

	i = 0;
	while (tests[i])
	{
		CHECK(ft_strlen(tests[i]) == strlen(tests[i]),
			"ft_strlen(\"%s\") = %zu", tests[i], ft_strlen(tests[i]));
		i++;
	}
	memset(big, 'x', 10000);
	big[10000] = '\0';
	CHECK(ft_strlen(big) == 10000, "ft_strlen(10000 caracteres)");
}

static void	t_strlcpy(void)
{
	const char	*srcs[] = {"", "a", "bonjour", "une chaine plus longue", NULL};
	size_t		sizes[] = {0, 1, 2, 5, 7, 8, 30};
	char		d1[40];
	char		d2[40];
	int			i;
	int			j;
	size_t		r1;
	size_t		r2;

	i = 0;
	while (srcs[i])
	{
		j = 0;
		while (j < 7)
		{
			memset(d1, 'Z', 40);
			memset(d2, 'Z', 40);
			r1 = ft_strlcpy(d1, srcs[i], sizes[j]);
			r2 = ref_strlcpy(d2, srcs[i], sizes[j]);
			CHECK(r1 == r2 && memcmp(d1, d2, 40) == 0,
				"ft_strlcpy(dst, \"%s\", %zu) : retour %zu (attendu %zu) ou dst different",
				srcs[i], sizes[j], r1, r2);
			j++;
		}
		i++;
	}
}

static void	t_strlcat(void)
{
	const char	*dsts[] = {"", "abc", "bonjour", NULL};
	const char	*srcs[] = {"", "xy", "une suite", NULL};
	size_t		sizes[] = {0, 1, 3, 4, 6, 10, 30};
	char		d1[40];
	char		d2[40];
	int			a;
	int			b;
	int			c;

	a = -1;
	while (dsts[++a])
	{
		b = -1;
		while (srcs[++b])
		{
			c = -1;
			while (++c < 7)
			{
				memset(d1, 'Z', 40);
				memset(d2, 'Z', 40);
				strcpy(d1, dsts[a]);
				strcpy(d2, dsts[a]);
				CHECK(ft_strlcat(d1, srcs[b], sizes[c])
					== ref_strlcat(d2, srcs[b], sizes[c])
					&& memcmp(d1, d2, 40) == 0,
					"ft_strlcat(\"%s\", \"%s\", %zu) : retour ou dst different",
					dsts[a], srcs[b], sizes[c]);
			}
		}
	}
}

static void	t_strchr_strrchr(void)
{
	const char	*s = "bonjour le monde";
	int			cs[] = {'b', 'o', 'e', 'z', '\0', 'b' + 256, 'e' + 256};
	int			i;

	i = 0;
	while (i < 7)
	{
		CHECK(ft_strchr(s, cs[i]) == strchr(s, cs[i]),
			"ft_strchr(\"%s\", %d)", s, cs[i]);
		CHECK(ft_strrchr(s, cs[i]) == strrchr(s, cs[i]),
			"ft_strrchr(\"%s\", %d)", s, cs[i]);
		i++;
	}
	CHECK(ft_strchr("", '\0') != NULL, "ft_strchr(\"\", '\\0') doit renvoyer la fin");
	CHECK(ft_strchr("", 'a') == NULL, "ft_strchr(\"\", 'a') doit renvoyer NULL");
	CHECK(ft_strrchr("", 'a') == NULL, "ft_strrchr(\"\", 'a') doit renvoyer NULL");
}

static void	t_strncmp(void)
{
	const char	*a[] = {"abc", "abc", "abc", "", "abc", "ab\200", "test", "a"};
	const char	*b[] = {"abc", "abd", "ab", "", "", "ab\0", "testing", "b"};
	size_t		n[] = {3, 3, 3, 1, 5, 3, 4, 0};
	int			i;

	i = 0;
	while (i < 8)
	{
		CHECK(sign(ft_strncmp(a[i], b[i], n[i])) == sign(strncmp(a[i], b[i], n[i])),
			"ft_strncmp(\"%s\", \"%s\", %zu) = %d, attendu le signe de %d",
			a[i], b[i], n[i], ft_strncmp(a[i], b[i], n[i]),
			strncmp(a[i], b[i], n[i]));
		i++;
	}
}

static void	t_strnstr(void)
{
	const char	*big = "trouver une aiguille dans la botte";
	const char	*lit[] = {"aiguille", "", "botte", "absent", "t", "trouver", "aiguille"};
	size_t		len[] = {40, 5, 40, 40, 0, 7, 15};
	int			i;

	i = 0;
	while (i < 7)
	{
		CHECK(ft_strnstr(big, lit[i], len[i]) == ref_strnstr(big, lit[i], len[i]),
			"ft_strnstr(big, \"%s\", %zu)", lit[i], len[i]);
		i++;
	}
	CHECK(ft_strnstr("", "", 0) != NULL, "ft_strnstr(\"\", \"\", 0) doit renvoyer big");
	CHECK(ft_strnstr("abc", "abcd", 10) == NULL, "ft_strnstr(\"abc\", \"abcd\", 10)");
}

static void	t_atoi(void)
{
	const char	*tests[] = {"0", "42", "-42", "+42", "   42", "\t\n\v\f\r 42",
		"42abc", "abc42", "--42", "+-42", "-+42", "- 42", "2147483647",
		"-2147483648", "", "   ", "+", "-", "0000042", "-0", NULL};
	int			i;

	i = 0;
	while (tests[i])
	{
		CHECK(ft_atoi(tests[i]) == atoi(tests[i]),
			"ft_atoi(\"%s\") = %d, attendu %d", tests[i], ft_atoi(tests[i]),
			atoi(tests[i]));
		i++;
	}
}

static void	t_memset_bzero(void)
{
	char	a[20];
	char	b[20];

	memset(a, 'A', 20);
	memset(b, 'A', 20);
	CHECK(ft_memset(a, 'x', 10) == a, "ft_memset doit renvoyer son pointeur");
	memset(b, 'x', 10);
	CHECK(memcmp(a, b, 20) == 0, "ft_memset(a, 'x', 10)");
	ft_memset(a, 300, 5);
	memset(b, 300, 5);
	CHECK(memcmp(a, b, 20) == 0, "ft_memset avec c = 300 (doit etre converti en unsigned char)");
	ft_memset(a, 'q', 0);
	CHECK(memcmp(a, b, 20) == 0, "ft_memset avec n = 0 ne doit rien changer");
	ft_bzero(a + 2, 6);
	memset(b + 2, 0, 6);
	CHECK(memcmp(a, b, 20) == 0, "ft_bzero(a + 2, 6)");
	ft_bzero(a, 0);
	CHECK(memcmp(a, b, 20) == 0, "ft_bzero avec n = 0 ne doit rien changer");
}

static void	t_memcpy_memmove(void)
{
	char	a[30];
	char	b[30];
	char	src[] = "0123456789abcdefghij";

	memset(a, 'Z', 30);
	CHECK(ft_memcpy(a, src, 10) == a, "ft_memcpy doit renvoyer dst");
	CHECK(memcmp(a, src, 10) == 0 && a[10] == 'Z', "ft_memcpy(a, src, 10)");
	CHECK(ft_memcpy(a, src, 0) == a, "ft_memcpy avec n = 0");
	strcpy(a, "0123456789");
	strcpy(b, "0123456789");
	CHECK(ft_memmove(a + 2, a, 6) == a + 2, "ft_memmove doit renvoyer dst");
	memmove(b + 2, b, 6);
	CHECK(memcmp(a, b, 11) == 0, "ft_memmove chevauchement (dst apres src) : %s au lieu de %s", a, b);
	strcpy(a, "0123456789");
	strcpy(b, "0123456789");
	ft_memmove(a, a + 3, 6);
	memmove(b, b + 3, 6);
	CHECK(memcmp(a, b, 11) == 0, "ft_memmove chevauchement (dst avant src) : %s au lieu de %s", a, b);
	strcpy(a, "0123456789");
	ft_memmove(a, a, 5);
	CHECK(strcmp(a, "0123456789") == 0, "ft_memmove(a, a, 5)");
}

static void	t_memchr_memcmp(void)
{
	char	s[] = "abc\0def\200";

	CHECK(ft_memchr(s, 'c', 9) == memchr(s, 'c', 9), "ft_memchr(s, 'c', 9)");
	CHECK(ft_memchr(s, 'e', 9) == memchr(s, 'e', 9), "ft_memchr apres un '\\0'");
	CHECK(ft_memchr(s, '\0', 9) == memchr(s, '\0', 9), "ft_memchr(s, '\\0', 9)");
	CHECK(ft_memchr(s, 'e', 3) == NULL, "ft_memchr(s, 'e', 3) doit renvoyer NULL");
	CHECK(ft_memchr(s, 'a' + 256, 9) == memchr(s, 'a' + 256, 9), "ft_memchr avec c = 'a' + 256");
	CHECK(ft_memchr(s, 0200, 9) == memchr(s, 0200, 9), "ft_memchr avec un caractere > 127");
	CHECK(ft_memcmp("abc", "abc", 3) == 0, "ft_memcmp egaux");
	CHECK(sign(ft_memcmp("abc", "abd", 3)) < 0, "ft_memcmp(\"abc\", \"abd\")");
	CHECK(sign(ft_memcmp("ab\200", "ab\0", 3)) > 0, "ft_memcmp doit comparer en unsigned char");
	CHECK(ft_memcmp("a\0b", "a\0c", 3) != 0, "ft_memcmp ne doit pas s'arreter au '\\0'");
	CHECK(ft_memcmp("abc", "xyz", 0) == 0, "ft_memcmp avec n = 0");
}

static void	t_calloc(void)
{
	unsigned char	*p;
	size_t			i;
	int				ok;

	p = ft_calloc(10, 4);
	CHECK(p != NULL, "ft_calloc(10, 4) a renvoye NULL");
	if (!p)
		return ;
	ok = 1;
	i = 0;
	while (i < 40)
		if (p[i++] != 0)
			ok = 0;
	CHECK(ok, "ft_calloc(10, 4) : la memoire n'est pas mise a 0");
	free(p);
	p = ft_calloc(0, 0);
	free(p);
}

static void	t_strdup(void)
{
	const char	*tests[] = {"", "a", "bonjour le monde", NULL};
	char		*d;
	int			i;

	i = 0;
	while (tests[i])
	{
		d = ft_strdup(tests[i]);
		CHECK(d && d != tests[i] && strcmp(d, tests[i]) == 0,
			"ft_strdup(\"%s\")", tests[i]);
		free(d);
		i++;
	}
}

/* ============================== PARTIE 2 ============================== */

static void	check_str(char *got, const char *expected, const char *call)
{
	char	b1[1024];
	char	b2[1024];

	CHECK(got && strcmp(got, expected) == 0, "%s = %s, attendu %s", call,
		show(got, got ? strlen(got) : 0, b1),
		show(expected, strlen(expected), b2));
	free(got);
}

static void	t_substr(void)
{
	check_str(ft_substr("bonjour", 0, 3), "bon", "ft_substr(\"bonjour\", 0, 3)");
	check_str(ft_substr("bonjour", 3, 4), "jour", "ft_substr(\"bonjour\", 3, 4)");
	check_str(ft_substr("bonjour", 3, 100), "jour", "ft_substr(\"bonjour\", 3, 100)");
	check_str(ft_substr("bonjour", 0, 0), "", "ft_substr(\"bonjour\", 0, 0)");
	check_str(ft_substr("bonjour", 7, 3), "", "ft_substr(\"bonjour\", 7, 3)");
	check_str(ft_substr("bonjour", 100, 3), "", "ft_substr(\"bonjour\", 100, 3)");
	check_str(ft_substr("", 0, 5), "", "ft_substr(\"\", 0, 5)");
}

static void	t_strjoin(void)
{
	check_str(ft_strjoin("bon", "jour"), "bonjour", "ft_strjoin(\"bon\", \"jour\")");
	check_str(ft_strjoin("", "jour"), "jour", "ft_strjoin(\"\", \"jour\")");
	check_str(ft_strjoin("bon", ""), "bon", "ft_strjoin(\"bon\", \"\")");
	check_str(ft_strjoin("", ""), "", "ft_strjoin(\"\", \"\")");
}

static void	t_strtrim(void)
{
	check_str(ft_strtrim("  bonjour  ", " "), "bonjour", "ft_strtrim(\"  bonjour  \", \" \")");
	check_str(ft_strtrim("xxhixx", "x"), "hi", "ft_strtrim(\"xxhixx\", \"x\")");
	check_str(ft_strtrim("abcHELLOcba", "abc"), "HELLO", "ft_strtrim(\"abcHELLOcba\", \"abc\")");
	check_str(ft_strtrim("a b", " "), "a b", "ft_strtrim(\"a b\", \" \")");
	check_str(ft_strtrim("     ", " "), "", "ft_strtrim(\"     \", \" \")");
	check_str(ft_strtrim("bonjour", ""), "bonjour", "ft_strtrim(\"bonjour\", \"\")");
	check_str(ft_strtrim("", "abc"), "", "ft_strtrim(\"\", \"abc\")");
}

static void	check_split(const char *s, char c, const char **expected)
{
	char	**r;
	int		i;
	int		ok;

	r = ft_split(s, c);
	CHECK(r != NULL, "ft_split(\"%s\", '%c') a renvoye NULL", s, c);
	if (!r)
		return ;
	ok = 1;
	i = 0;
	while (expected[i] && r[i] && strcmp(expected[i], r[i]) == 0)
		i++;
	if (expected[i] || r[i])
		ok = 0;
	CHECK(ok, "ft_split(\"%s\", '%c') : mot %d = \"%s\", attendu \"%s\"",
		s, c ? c : '0', i, r[i] ? r[i] : "NULL",
		expected[i] ? expected[i] : "NULL");
	i = 0;
	while (r[i])
		free(r[i++]);
	free(r);
}

static void	t_split(void)
{
	const char	*e1[] = {"hello", "world", "42", NULL};
	const char	*e2[] = {NULL};
	const char	*e3[] = {"abc", NULL};
	const char	*e4[] = {"x", "yy", "z", NULL};
	const char	*e5[] = {"bonjour", NULL};

	check_split("hello world 42", ' ', e1);
	check_split("   hello   world  42   ", ' ', e1);
	check_split("", ' ', e2);
	check_split("     ", ' ', e2);
	check_split("abc", ' ', e3);
	check_split(",,x,,yy,z,", ',', e4);
	check_split("bonjour", '\0', e5);
}

static void	t_itoa(void)
{
	int		v[] = {0, 1, -1, 9, 10, -10, 42, -42, 123456789, INT_MAX, INT_MIN};
	char	expected[20];
	char	call[40];
	int		i;

	i = 0;
	while (i < 11)
	{
		sprintf(expected, "%d", v[i]);
		sprintf(call, "ft_itoa(%d)", v[i]);
		check_str(ft_itoa(v[i]), expected, call);
		i++;
	}
}

static char	map_upper_even(unsigned int i, char c)
{
	if (i % 2 == 0)
		return (toupper(c));
	return (c);
}

static void	iter_upper_even(unsigned int i, char *c)
{
	if (i % 2 == 0)
		*c = toupper(*c);
}

static void	t_strmapi_striteri(void)
{
	char	s[] = "bonjour";
	char	e[] = "";

	check_str(ft_strmapi("bonjour", map_upper_even), "BoNjOuR",
		"ft_strmapi(\"bonjour\", f)");
	check_str(ft_strmapi("", map_upper_even), "", "ft_strmapi(\"\", f)");
	ft_striteri(s, iter_upper_even);
	CHECK(strcmp(s, "BoNjOuR") == 0, "ft_striteri(\"bonjour\", f) = \"%s\", attendu \"BoNjOuR\"", s);
	ft_striteri(e, iter_upper_even);
}

static void	t_put_fd(void)
{
	int		p[2];
	char	buf[256];
	char	b1[1024];
	size_t	len;

	if (pipe(p) == -1)
		return ;
	ft_putchar_fd('A', p[1]);
	ft_putstr_fd("bonjour", p[1]);
	ft_putendl_fd("fin", p[1]);
	ft_putstr_fd("", p[1]);
	ft_putendl_fd("", p[1]);
	ft_putnbr_fd(0, p[1]);
	ft_putchar_fd('|', p[1]);
	ft_putnbr_fd(-42, p[1]);
	ft_putchar_fd('|', p[1]);
	ft_putnbr_fd(10, p[1]);
	ft_putchar_fd('|', p[1]);
	ft_putnbr_fd(INT_MAX, p[1]);
	ft_putchar_fd('|', p[1]);
	ft_putnbr_fd(INT_MIN, p[1]);
	close(p[1]);
	len = read_pipe(p[0], buf, sizeof(buf));
	close(p[0]);
	CHECK(strcmp(buf, "Abonjourfin\n\n0|-42|10|2147483647|-2147483648") == 0,
		"ft_put*_fd : obtenu %s", show(buf, len, b1));
}

/* ============================== PARTIE 3 ============================== */

static int	g_del_count = 0;

static void	del_count(void *p)
{
	g_del_count++;
	free(p);
}

static void	*map_dup_upper(void *p)
{
	char	*s;
	int		i;

	s = strdup((char *)p);
	i = 0;
	while (s && s[i])
	{
		s[i] = toupper(s[i]);
		i++;
	}
	return (s);
}

static void	iter_upper(void *p)
{
	char	*s;

	s = (char *)p;
	while (*s)
	{
		*s = toupper(*s);
		s++;
	}
}

static void	t_lstnew(void)
{
	int		x;
	t_list	*n;

	x = 42;
	n = ft_lstnew(&x);
	CHECK(n && n->content == &x && n->next == NULL,
		"ft_lstnew : content ou next incorrect");
	free(n);
	n = ft_lstnew(NULL);
	CHECK(n && n->content == NULL && n->next == NULL,
		"ft_lstnew(NULL) doit creer un noeud avec content = NULL");
	free(n);
}

static void	t_lstadd_size_last(void)
{
	t_list	*lst;
	t_list	*a;
	t_list	*b;
	t_list	*c;

	lst = NULL;
	CHECK(ft_lstsize(NULL) == 0, "ft_lstsize(NULL) doit valoir 0");
	CHECK(ft_lstlast(NULL) == NULL, "ft_lstlast(NULL) doit valoir NULL");
	a = ft_lstnew("a");
	b = ft_lstnew("b");
	c = ft_lstnew("c");
	ft_lstadd_back(&lst, b);
	CHECK(lst == b, "ft_lstadd_back sur une liste vide");
	ft_lstadd_front(&lst, a);
	CHECK(lst == a && a->next == b, "ft_lstadd_front");
	ft_lstadd_back(&lst, c);
	CHECK(b->next == c && c->next == NULL, "ft_lstadd_back a la fin");
	CHECK(ft_lstsize(lst) == 3, "ft_lstsize = %u, attendu 3", ft_lstsize(lst));
	CHECK(ft_lstlast(lst) == c, "ft_lstlast");
	CHECK(ft_lstsize(c) == 1, "ft_lstsize d'un seul noeud");
	free(a);
	free(b);
	free(c);
}

static void	t_lstdelone_clear(void)
{
	t_list	*lst;
	t_list	*n;

	g_del_count = 0;
	n = ft_lstnew(strdup("seul"));
	ft_lstdelone(n, del_count);
	CHECK(g_del_count == 1, "ft_lstdelone doit appeler del une fois");
	lst = NULL;
	ft_lstadd_back(&lst, ft_lstnew(strdup("1")));
	ft_lstadd_back(&lst, ft_lstnew(strdup("2")));
	ft_lstadd_back(&lst, ft_lstnew(strdup("3")));
	g_del_count = 0;
	ft_lstclear(&lst, del_count);
	CHECK(g_del_count == 3, "ft_lstclear : del appele %d fois, attendu 3", g_del_count);
	CHECK(lst == NULL, "ft_lstclear doit mettre la liste a NULL");
	ft_lstclear(&lst, del_count);
}

static void	t_lstiter_map(void)
{
	t_list	*lst;
	t_list	*m;
	char	s1[] = "un";
	char	s2[] = "deux";

	lst = NULL;
	ft_lstadd_back(&lst, ft_lstnew(s1));
	ft_lstadd_back(&lst, ft_lstnew(s2));
	m = ft_lstmap(lst, map_dup_upper, free);
	CHECK(m && ft_lstsize(m) == 2 && strcmp(m->content, "UN") == 0
		&& strcmp(m->next->content, "DEUX") == 0, "ft_lstmap");
	CHECK(strcmp(s1, "un") == 0, "ft_lstmap ne doit pas modifier la liste d'origine");
	ft_lstclear(&m, free);
	ft_lstiter(lst, iter_upper);
	CHECK(strcmp(s1, "UN") == 0 && strcmp(s2, "DEUX") == 0, "ft_lstiter");
	CHECK(ft_lstmap(NULL, map_dup_upper, free) == NULL, "ft_lstmap(NULL) doit renvoyer NULL");
	free(lst->next);
	free(lst);
}

int	main(void)
{
	print_title("Libft - Partie 1 : fonctions de la libc");
	run_test("ft_isalpha / isdigit / isalnum / isascii / isprint", t_isfuncs);
	run_test("ft_toupper / ft_tolower", t_toupper_tolower);
	run_test("ft_strlen", t_strlen);
	run_test("ft_strlcpy", t_strlcpy);
	run_test("ft_strlcat", t_strlcat);
	run_test("ft_strchr / ft_strrchr", t_strchr_strrchr);
	run_test("ft_strncmp", t_strncmp);
	run_test("ft_strnstr", t_strnstr);
	run_test("ft_atoi", t_atoi);
	run_test("ft_memset / ft_bzero", t_memset_bzero);
	run_test("ft_memcpy / ft_memmove", t_memcpy_memmove);
	run_test("ft_memchr / ft_memcmp", t_memchr_memcmp);
	run_test("ft_calloc", t_calloc);
	run_test("ft_strdup", t_strdup);
	print_title("Libft - Partie 2 : fonctions supplementaires");
	run_test("ft_substr", t_substr);
	run_test("ft_strjoin", t_strjoin);
	run_test("ft_strtrim", t_strtrim);
	run_test("ft_split", t_split);
	run_test("ft_itoa", t_itoa);
	run_test("ft_strmapi / ft_striteri", t_strmapi_striteri);
	run_test("ft_putchar_fd / putstr / putendl / putnbr", t_put_fd);
	print_title("Libft - Partie 3 : listes chainees");
	run_test("ft_lstnew", t_lstnew);
	run_test("ft_lstadd_front / back / size / last", t_lstadd_size_last);
	run_test("ft_lstdelone / ft_lstclear", t_lstdelone_clear);
	run_test("ft_lstiter / ft_lstmap", t_lstiter_map);
	return (print_summary());
}
