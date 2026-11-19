/*
** test_libft.c - testeur de la Libft
** Lance-le avec ./run.sh (il compile la libft et ce fichier).
** Chaque verification est ecrite dans le log (variable LOG_FILE).
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

/* decrit un pointeur renvoye par rapport au debut de la chaine */
static char	*where(const char *base, const char *p, char *out)
{
	if (!p)
		return (strcpy(out, "NULL"));
	sprintf(out, "s+%td", p - base);
	return (out);
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

/*
** Compare une fonction ft_isxxx avec celle de la libc pour c de -1 a 255.
** Une seule ligne dans le log, avec les valeurs fausses s'il y en a.
*/
static void	check_is(const char *name, int (*ft)(int), int (*ref)(int))
{
	char	bad[1024];
	int		nb;
	int		c;
	size_t	j;

	nb = 0;
	j = 0;
	bad[0] = '\0';
	c = -1;
	while (c <= 255)
	{
		if (!!ft(c) != !!ref(c))
		{
			if (nb < 8)
				j += sprintf(bad + j, " c=%d (obtenu %d, attendu %s)", c, ft(c),
						ref(c) ? "non nul" : "0");
			nb++;
		}
		c++;
	}
	CHECK(nb == 0, "%s(c) pour c de -1 a 255, compare a la libc : %d erreur(s)%s",
		name, nb, bad);
}

static int	ref_isascii(int c)
{
	return (c >= 0 && c <= 127);
}

static void	t_isfuncs(void)
{
	check_is("ft_isalpha", ft_isalpha, isalpha);
	check_is("ft_isdigit", ft_isdigit, isdigit);
	check_is("ft_isalnum", ft_isalnum, isalnum);
	check_is("ft_isascii", ft_isascii, ref_isascii);
	check_is("ft_isprint", ft_isprint, isprint);
}

static void	check_case(const char *name, int (*ft)(int), int (*ref)(int))
{
	char	bad[1024];
	int		nb;
	int		c;
	size_t	j;

	nb = 0;
	j = 0;
	bad[0] = '\0';
	c = -1;
	while (c <= 255)
	{
		if (ft(c) != ref(c))
		{
			if (nb < 8)
				j += sprintf(bad + j, " c=%d (obtenu %d, attendu %d)", c, ft(c),
						ref(c));
			nb++;
		}
		c++;
	}
	CHECK(nb == 0, "%s(c) pour c de -1 a 255, compare a la libc : %d erreur(s)%s",
		name, nb, bad);
}

static void	t_toupper_tolower(void)
{
	check_case("ft_toupper", ft_toupper, toupper);
	check_case("ft_tolower", ft_tolower, tolower);
}

static void	t_strlen(void)
{
	const char	*tests[] = {"", "a", "ab", "bonjour", "avec\tdes\nespaces",
		"\200\377 caracteres > 127", NULL};
	char		big[10001];
	char		b1[1024];
	int			i;

	i = 0;
	while (tests[i])
	{
		CHECK(ft_strlen(tests[i]) == strlen(tests[i]),
			"ft_strlen(%s) = %zu, attendu %zu",
			show(tests[i], strlen(tests[i]), b1), ft_strlen(tests[i]),
			strlen(tests[i]));
		i++;
	}
	memset(big, 'x', 10000);
	big[10000] = '\0';
	CHECK(ft_strlen(big) == 10000, "ft_strlen(chaine de 10000 'x') = %zu, attendu 10000",
		ft_strlen(big));
}

static void	t_strlcpy(void)
{
	const char	*srcs[] = {"", "a", "bonjour", "une chaine un peu plus longue", NULL};
	size_t		sizes[] = {0, 1, 2, 5, 7, 8, 9, 30, 35};
	char		d1[40];
	char		d2[40];
	char		b1[1024];
	char		b2[1024];
	int			i;
	int			j;
	size_t		r1;
	size_t		r2;

	i = -1;
	while (srcs[++i])
	{
		j = -1;
		while (++j < 9)
		{
			memset(d1, 'Z', 40);
			memset(d2, 'Z', 40);
			r1 = ft_strlcpy(d1, srcs[i], sizes[j]);
			r2 = ref_strlcpy(d2, srcs[i], sizes[j]);
			CHECK(r1 == r2 && memcmp(d1, d2, 40) == 0,
				"ft_strlcpy(dst, \"%s\", %zu) : retour %zu (attendu %zu), dst = %s (attendu %s)",
				srcs[i], sizes[j], r1, r2, show(d1, strnlen(d1, 39), b1),
				show(d2, strnlen(d2, 39), b2));
		}
	}
}

static void	t_strlcat(void)
{
	const char	*dsts[] = {"", "abc", "bonjour", NULL};
	const char	*srcs[] = {"", "xy", "une suite", NULL};
	size_t		sizes[] = {0, 1, 2, 3, 4, 6, 8, 10, 11, 30};
	char		d1[40];
	char		d2[40];
	char		b1[1024];
	char		b2[1024];
	size_t		r1;
	size_t		r2;
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
			while (++c < 10)
			{
				memset(d1, 'Z', 40);
				memset(d2, 'Z', 40);
				strcpy(d1, dsts[a]);
				strcpy(d2, dsts[a]);
				r1 = ft_strlcat(d1, srcs[b], sizes[c]);
				r2 = ref_strlcat(d2, srcs[b], sizes[c]);
				CHECK(r1 == r2 && memcmp(d1, d2, 40) == 0,
					"ft_strlcat(\"%s\", \"%s\", %zu) : retour %zu (attendu %zu), dst = %s (attendu %s)",
					dsts[a], srcs[b], sizes[c], r1, r2,
					show(d1, strnlen(d1, 39), b1), show(d2, strnlen(d2, 39), b2));
			}
		}
	}
}

static void	t_strchr_strrchr(void)
{
	const char	*strs[] = {"bonjour le monde", "", "a", "aaaa", "\200\377x", NULL};
	int			cs[] = {'b', 'o', 'e', 'a', 'z', '\0', 'b' + 256, 'e' + 256,
		'a' - 256, 0200, 0377, -1};
	char		w1[64];
	char		w2[64];
	int			i;
	int			k;

	i = -1;
	while (strs[++i])
	{
		k = -1;
		while (++k < 12)
		{
			CHECK(ft_strchr(strs[i], cs[k]) == strchr(strs[i], cs[k]),
				"ft_strchr(\"%s\", %d) = %s, attendu %s", strs[i], cs[k],
				where(strs[i], ft_strchr(strs[i], cs[k]), w1),
				where(strs[i], strchr(strs[i], cs[k]), w2));
			CHECK(ft_strrchr(strs[i], cs[k]) == strrchr(strs[i], cs[k]),
				"ft_strrchr(\"%s\", %d) = %s, attendu %s", strs[i], cs[k],
				where(strs[i], ft_strrchr(strs[i], cs[k]), w1),
				where(strs[i], strrchr(strs[i], cs[k]), w2));
		}
	}
}

static void	t_strncmp(void)
{
	const char	*a[] = {"abc", "abc", "abc", "", "abc", "", "ab\200", "\200",
		"test", "a", "abcdef", "abc", "\377", "abc", "abc\0zz", "zzz"};
	const char	*b[] = {"abc", "abd", "ab", "", "", "abc", "ab\0", "a",
		"testing", "b", "abcxyz", "abcd", "\001", "ABC", "abc\0yy", "zz"};
	size_t		n[] = {3, 3, 3, 1, 5, 2, 3, 1, 4, 0, 3, 100, 1, 3, 6, 2};
	int			i;

	i = 0;
	while (i < 16)
	{
		CHECK(sign(ft_strncmp(a[i], b[i], n[i])) == sign(strncmp(a[i], b[i], n[i])),
			"ft_strncmp(\"%s\", \"%s\", %zu) = %d, attendu un resultat du signe de %d",
			a[i], b[i], n[i], ft_strncmp(a[i], b[i], n[i]),
			strncmp(a[i], b[i], n[i]));
		i++;
	}
}

static void	t_strnstr(void)
{
	const char	*big = "trouver une aiguille dans la botte de foin";
	const char	*lit[] = {"aiguille", "", "botte", "absent", "t", "trouver",
		"aiguille", "foin", "foin", "n", "ille d", "aiguilles"};
	size_t		len[] = {50, 5, 50, 50, 0, 7, 15, 42, 41, 1, 50, 50};
	char		w1[64];
	char		w2[64];
	int			i;

	i = 0;
	while (i < 12)
	{
		CHECK(ft_strnstr(big, lit[i], len[i]) == ref_strnstr(big, lit[i], len[i]),
			"ft_strnstr(\"%s\", \"%s\", %zu) = %s, attendu %s", big, lit[i], len[i],
			where(big, ft_strnstr(big, lit[i], len[i]), w1),
			where(big, ref_strnstr(big, lit[i], len[i]), w2));
		i++;
	}
	CHECK(ft_strnstr("aaab", "aab", 4) == ref_strnstr("aaab", "aab", 4),
		"ft_strnstr(\"aaab\", \"aab\", 4) doit trouver \"aab\" en position 1");
	CHECK(ft_strnstr("", "", 0) != NULL, "ft_strnstr(\"\", \"\", 0) doit renvoyer big (obtenu NULL)");
	CHECK(ft_strnstr("", "a", 5) == NULL, "ft_strnstr(\"\", \"a\", 5) doit renvoyer NULL");
	CHECK(ft_strnstr("abc", "abcd", 10) == NULL, "ft_strnstr(\"abc\", \"abcd\", 10) doit renvoyer NULL");
}

static void	t_atoi(void)
{
	const char	*tests[] = {"0", "1", "-1", "42", "-42", "+42", "   42",
		"\t\n\v\f\r 42", "42abc", "abc42", "--42", "+-42", "-+42", "- 42",
		"+ 42", "2147483647", "-2147483648", "", "   ", "+", "-", "0000042",
		"-0", "4 2", "12\n34", "\x01 42", "99999", "-100000", " \t-7x", NULL};
	char		b1[1024];
	int			i;

	i = 0;
	while (tests[i])
	{
		CHECK(ft_atoi(tests[i]) == atoi(tests[i]),
			"ft_atoi(%s) = %d, attendu %d", show(tests[i], strlen(tests[i]), b1),
			ft_atoi(tests[i]), atoi(tests[i]));
		i++;
	}
}

static void	t_memset_bzero(void)
{
	char	a[32];
	char	b[32];
	int		vals[] = {'x', 0, 255, 300, -1, 'A' + 256};
	int		i;

	i = -1;
	while (++i < 6)
	{
		memset(a, '.', 32);
		memset(b, '.', 32);
		CHECK(ft_memset(a + 3, vals[i], 10) == a + 3,
			"ft_memset(s, %d, 10) doit renvoyer s", vals[i]);
		memset(b + 3, vals[i], 10);
		CHECK(memcmp(a, b, 32) == 0,
			"ft_memset(s, %d, 10) : contenu different de memset (c doit etre converti en unsigned char, rien ne doit etre ecrit hors des 10 octets)",
			vals[i]);
	}
	memset(a, '.', 32);
	memset(b, '.', 32);
	ft_memset(a, 'q', 0);
	CHECK(memcmp(a, b, 32) == 0, "ft_memset(s, 'q', 0) ne doit rien changer");
	ft_bzero(a + 2, 6);
	memset(b + 2, 0, 6);
	CHECK(memcmp(a, b, 32) == 0, "ft_bzero(s + 2, 6) : seuls les octets 2 a 7 doivent valoir 0");
	ft_bzero(a, 0);
	CHECK(memcmp(a, b, 32) == 0, "ft_bzero(s, 0) ne doit rien changer");
}

static void	t_memcpy_memmove(void)
{
	char	a[64];
	char	b[64];
	char	src[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	size_t	offs[] = {1, 2, 5, 10};
	size_t	ns[] = {0, 1, 5, 20};
	int		i;
	int		j;

	memset(a, 'Z', 64);
	CHECK(ft_memcpy(a, src, 10) == a, "ft_memcpy(dst, src, 10) doit renvoyer dst");
	CHECK(memcmp(a, src, 10) == 0 && a[10] == 'Z',
		"ft_memcpy(dst, src, 10) : les 10 octets doivent etre copies, et rien apres");
	CHECK(ft_memcpy(a, src, 0) == a, "ft_memcpy(dst, src, 0) doit renvoyer dst");
	memcpy(b, "a\0b\0c", 5);
	ft_memcpy(a, b, 5);
	CHECK(memcmp(a, "a\0b\0c", 5) == 0, "ft_memcpy ne doit pas s'arreter aux '\\0'");
	memset(a, 0, 64);
	memset(b, 0, 64);
	i = -1;
	while (++i < 4)
	{
		j = -1;
		while (++j < 4)
		{
			strcpy(a, src);
			strcpy(b, src);
			CHECK(ft_memmove(a + offs[i], a, ns[j]) == a + offs[i],
				"ft_memmove doit renvoyer dst");
			memmove(b + offs[i], b, ns[j]);
			CHECK(memcmp(a, b, 40) == 0,
				"ft_memmove(s + %zu, s, %zu) (dst apres src, zones qui se chevauchent) : %s au lieu de %s",
				offs[i], ns[j], a, b);
			strcpy(a, src);
			strcpy(b, src);
			ft_memmove(a, a + offs[i], ns[j]);
			memmove(b, b + offs[i], ns[j]);
			CHECK(memcmp(a, b, 40) == 0,
				"ft_memmove(s, s + %zu, %zu) (dst avant src) : %s au lieu de %s",
				offs[i], ns[j], a, b);
		}
	}
	strcpy(a, src);
	ft_memmove(a, a, 10);
	CHECK(strcmp(a, src) == 0, "ft_memmove(s, s, 10) ne doit rien changer");
}

static void	t_memchr_memcmp(void)
{
	char	s[] = "abc\0def\200\377";
	int		cs[] = {'a', 'c', 'e', '\0', 'z', 'a' + 256, 0200, 0377, -1};
	size_t	ns[] = {0, 3, 4, 10};
	char	w1[64];
	char	w2[64];
	int		i;
	int		j;

	i = -1;
	while (++i < 9)
	{
		j = -1;
		while (++j < 4)
			CHECK(ft_memchr(s, cs[i], ns[j]) == memchr(s, cs[i], ns[j]),
				"ft_memchr(\"abc\\0def\\x80\\xff\", %d, %zu) = %s, attendu %s",
				cs[i], ns[j], where(s, ft_memchr(s, cs[i], ns[j]), w1),
				where(s, memchr(s, cs[i], ns[j]), w2));
	}
	CHECK(ft_memcmp("abc", "abc", 3) == 0, "ft_memcmp(\"abc\", \"abc\", 3) = %d, attendu 0",
		ft_memcmp("abc", "abc", 3));
	CHECK(sign(ft_memcmp("abc", "abd", 3)) < 0, "ft_memcmp(\"abc\", \"abd\", 3) = %d, attendu < 0",
		ft_memcmp("abc", "abd", 3));
	CHECK(sign(ft_memcmp("abd", "abc", 3)) > 0, "ft_memcmp(\"abd\", \"abc\", 3) = %d, attendu > 0",
		ft_memcmp("abd", "abc", 3));
	CHECK(sign(ft_memcmp("ab\200", "ab\0", 3)) > 0,
		"ft_memcmp(\"ab\\x80\", \"ab\\0\", 3) = %d, attendu > 0 (comparaison en unsigned char)",
		ft_memcmp("ab\200", "ab\0", 3));
	CHECK(ft_memcmp("a\0b", "a\0c", 3) != 0,
		"ft_memcmp(\"a\\0b\", \"a\\0c\", 3) = 0 : il ne faut pas s'arreter au '\\0'");
	CHECK(ft_memcmp("abc", "xyz", 0) == 0, "ft_memcmp(\"abc\", \"xyz\", 0) = %d, attendu 0",
		ft_memcmp("abc", "xyz", 0));
	CHECK(sign(ft_memcmp("abcX", "abcY", 3)) == 0,
		"ft_memcmp(\"abcX\", \"abcY\", 3) doit comparer seulement 3 octets");
}

static void	t_calloc(void)
{
	size_t			counts[] = {1, 10, 3, 1000};
	size_t			sizes[] = {1, 4, 8, 1};
	unsigned char	*p;
	size_t			i;
	size_t			k;
	int				ok;

	k = 0;
	while (k < 4)
	{
		p = ft_calloc(counts[k], sizes[k]);
		CHECK(p != NULL, "ft_calloc(%zu, %zu) a renvoye NULL", counts[k], sizes[k]);
		if (p)
		{
			ok = 1;
			i = 0;
			while (i < counts[k] * sizes[k])
				if (p[i++] != 0)
					ok = 0;
			CHECK(ok, "ft_calloc(%zu, %zu) : les %zu octets doivent valoir 0",
				counts[k], sizes[k], counts[k] * sizes[k]);
			free(p);
		}
		k++;
	}
	p = ft_calloc(0, 0);
	CHECK(1, "ft_calloc(0, 0) ne plante pas (renvoie %s)", p ? "un pointeur" : "NULL");
	free(p);
	p = ft_calloc(SIZE_MAX, 2);
	CHECK(p == NULL,
		"ft_calloc(SIZE_MAX, 2) : count * size depasse SIZE_MAX, doit renvoyer NULL comme calloc");
	free(p);
}

static void	t_strdup(void)
{
	const char	*tests[] = {"", "a", "bonjour le monde", "avec\nretour", NULL};
	char		big[5001];
	char		*d;
	char		b1[1024];
	int			i;

	i = 0;
	while (tests[i])
	{
		d = ft_strdup(tests[i]);
		CHECK(d && d != tests[i] && strcmp(d, tests[i]) == 0,
			"ft_strdup(%s) = %s", show(tests[i], strlen(tests[i]), b1),
			d ? d : "NULL");
		free(d);
		i++;
	}
	memset(big, 'k', 5000);
	big[5000] = '\0';
	d = ft_strdup(big);
	CHECK(d && strcmp(d, big) == 0, "ft_strdup(chaine de 5000 caracteres)");
	free(d);
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
	check_str(ft_substr("bonjour", 0, 7), "bonjour", "ft_substr(\"bonjour\", 0, 7)");
	check_str(ft_substr("bonjour", 6, 1), "r", "ft_substr(\"bonjour\", 6, 1)");
	check_str(ft_substr("bonjour", 0, 0), "", "ft_substr(\"bonjour\", 0, 0)");
	check_str(ft_substr("bonjour", 7, 3), "", "ft_substr(\"bonjour\", 7, 3)");
	check_str(ft_substr("bonjour", 100, 3), "", "ft_substr(\"bonjour\", 100, 3)");
	check_str(ft_substr("bonjour", 2, SIZE_MAX), "njour", "ft_substr(\"bonjour\", 2, SIZE_MAX)");
	check_str(ft_substr("", 0, 5), "", "ft_substr(\"\", 0, 5)");
	check_str(ft_substr("", 1, 5), "", "ft_substr(\"\", 1, 5)");
}

static void	t_strjoin(void)
{
	char	big[2001];
	char	expected[4001];

	check_str(ft_strjoin("bon", "jour"), "bonjour", "ft_strjoin(\"bon\", \"jour\")");
	check_str(ft_strjoin("", "jour"), "jour", "ft_strjoin(\"\", \"jour\")");
	check_str(ft_strjoin("bon", ""), "bon", "ft_strjoin(\"bon\", \"\")");
	check_str(ft_strjoin("", ""), "", "ft_strjoin(\"\", \"\")");
	check_str(ft_strjoin("a\nb", "\nc"), "a\nb\nc", "ft_strjoin(\"a\\nb\", \"\\nc\")");
	memset(big, 'q', 2000);
	big[2000] = '\0';
	strcpy(expected, big);
	strcat(expected, big);
	check_str(ft_strjoin(big, big), expected, "ft_strjoin(2000 caracteres, 2000 caracteres)");
}

static void	t_strtrim(void)
{
	check_str(ft_strtrim("  bonjour  ", " "), "bonjour", "ft_strtrim(\"  bonjour  \", \" \")");
	check_str(ft_strtrim("xxhixx", "x"), "hi", "ft_strtrim(\"xxhixx\", \"x\")");
	check_str(ft_strtrim("abcHELLOcba", "abc"), "HELLO", "ft_strtrim(\"abcHELLOcba\", \"abc\")");
	check_str(ft_strtrim("a b", " "), "a b", "ft_strtrim(\"a b\", \" \")");
	check_str(ft_strtrim("xxaxbxx", "x"), "axb", "ft_strtrim(\"xxaxbxx\", \"x\") (le milieu ne change pas)");
	check_str(ft_strtrim("     ", " "), "", "ft_strtrim(\"     \", \" \")");
	check_str(ft_strtrim("bonjour", ""), "bonjour", "ft_strtrim(\"bonjour\", \"\")");
	check_str(ft_strtrim("", "abc"), "", "ft_strtrim(\"\", \"abc\")");
	check_str(ft_strtrim("", ""), "", "ft_strtrim(\"\", \"\")");
	check_str(ft_strtrim(" \t\n x \n\t ", " \t\n"), "x", "ft_strtrim(\" \\t\\n x \\n\\t \", \" \\t\\n\")");
	check_str(ft_strtrim("abc", "xyz"), "abc", "ft_strtrim(\"abc\", \"xyz\")");
	check_str(ft_strtrim("aaaa", "a"), "", "ft_strtrim(\"aaaa\", \"a\")");
}

static void	check_split(const char *s, char c, const char **expected)
{
	char	**r;
	char	b1[1024];
	int		i;
	int		ok;

	r = ft_split(s, c);
	CHECK(r != NULL, "ft_split(%s, '%c') a renvoye NULL",
		show(s, strlen(s), b1), c ? c : '0');
	if (!r)
		return ;
	ok = 1;
	i = 0;
	while (expected[i] && r[i] && strcmp(expected[i], r[i]) == 0)
		i++;
	if (expected[i] || r[i])
		ok = 0;
	if (ok)
		CHECK(1, "ft_split(%s, '%c') : %d mot(s), tous corrects, tableau termine par NULL",
			show(s, strlen(s), b1), c ? c : '0', i);
	else
		CHECK(0, "ft_split(%s, '%c') : mot %d = \"%s\", attendu \"%s\"",
			show(s, strlen(s), b1), c ? c : '0', i, r[i] ? r[i] : "NULL",
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
	const char	*e6[] = {"a", "b", "c", "d", "e", NULL};
	const char	*e7[] = {"lorem", "ipsum", "dolor", "sit", "amet,", NULL};
	char		big[2001];
	char		**r;
	int			i;

	check_split("hello world 42", ' ', e1);
	check_split("   hello   world  42   ", ' ', e1);
	check_split("", ' ', e2);
	check_split("     ", ' ', e2);
	check_split("abc", ' ', e3);
	check_split("abc", 'z', e3);
	check_split(",,x,,yy,z,", ',', e4);
	check_split("bonjour", '\0', e5);
	check_split("a b c d e", ' ', e6);
	check_split("lorem ipsum dolor sit amet,", ' ', e7);
	check_split("xxxxabcxxxx", 'x', e3);
	memset(big, 'w', 2000);
	i = 0;
	while (i < 2000)
	{
		big[i] = ' ';
		i += 4;
	}
	big[2000] = '\0';
	r = ft_split(big, ' ');
	i = 0;
	while (r && r[i] && strcmp(r[i], "www") == 0)
		i++;
	CHECK(r && i == 500 && r[i] == NULL,
		"ft_split(500 mots \"www\" separes par des espaces) : %d mots corrects", i);
	i = 0;
	while (r && r[i])
		free(r[i++]);
	free(r);
}

static void	t_itoa(void)
{
	int		v[] = {0, 1, -1, 5, -5, 9, -9, 10, -10, 42, -42, 99, -99, 100, -100,
		1000, 12345, -12345, 99999, 100000, 123456789, -123456789, 999999999,
		-999999999, 1000000000, INT_MAX, INT_MIN, INT_MAX - 1, INT_MIN + 1};
	char	expected[20];
	char	call[40];
	int		i;

	i = 0;
	while (i < (int)(sizeof(v) / sizeof(*v)))
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

static char	map_add_index(unsigned int i, char c)
{
	return (c + i);
}

static void	iter_upper_even(unsigned int i, char *c)
{
	if (i % 2 == 0)
		*c = toupper(*c);
}

static void	iter_add_index(unsigned int i, char *c)
{
	*c = *c + i;
}

static void	t_strmapi_striteri(void)
{
	char	s[] = "bonjour";
	char	z[] = "aaaaa";
	char	e[] = "";
	char	*src;

	src = "bonjour";
	check_str(ft_strmapi(src, map_upper_even), "BoNjOuR",
		"ft_strmapi(\"bonjour\", met en majuscule les indices pairs)");
	CHECK(strcmp(src, "bonjour") == 0, "ft_strmapi ne doit pas modifier la chaine d'origine");
	check_str(ft_strmapi("aaaaa", map_add_index), "abcde",
		"ft_strmapi(\"aaaaa\", c + index) (verifie que l'index passe a f est le bon)");
	check_str(ft_strmapi("", map_upper_even), "", "ft_strmapi(\"\", f)");
	ft_striteri(s, iter_upper_even);
	CHECK(strcmp(s, "BoNjOuR") == 0,
		"ft_striteri(\"bonjour\", met en majuscule les indices pairs) = \"%s\", attendu \"BoNjOuR\"", s);
	ft_striteri(z, iter_add_index);
	CHECK(strcmp(z, "abcde") == 0,
		"ft_striteri(\"aaaaa\", c + index) = \"%s\", attendu \"abcde\"", z);
	ft_striteri(e, iter_upper_even);
	CHECK(e[0] == '\0', "ft_striteri(\"\", f) ne doit rien faire");
}

/* appelle une fonction put*_fd sur un pipe et compare ce qui a ete ecrit */
#define PUT_TEST(call, expected) \
	do { \
		int		p_[2]; \
		char	buf_[256]; \
		char	b1_[1024]; \
		char	b2_[1024]; \
		size_t	len_; \
		if (pipe(p_) == 0) \
		{ \
			call; \
			close(p_[1]); \
			len_ = read_pipe(p_[0], buf_, sizeof(buf_)); \
			close(p_[0]); \
			CHECK(len_ == sizeof(expected) - 1 \
				&& memcmp(buf_, expected, len_) == 0, \
				"%s ecrit %s, attendu %s", #call, show(buf_, len_, b1_), \
				show(expected, sizeof(expected) - 1, b2_)); \
		} \
	} while (0)

static void	t_put_fd(void)
{
	PUT_TEST(ft_putchar_fd('A', p_[1]), "A");
	PUT_TEST(ft_putchar_fd('\n', p_[1]), "\n");
	PUT_TEST(ft_putchar_fd('\0', p_[1]), "\0");
	PUT_TEST(ft_putstr_fd("bonjour", p_[1]), "bonjour");
	PUT_TEST(ft_putstr_fd("", p_[1]), "");
	PUT_TEST(ft_putstr_fd("a\nb", p_[1]), "a\nb");
	PUT_TEST(ft_putendl_fd("fin", p_[1]), "fin\n");
	PUT_TEST(ft_putendl_fd("", p_[1]), "\n");
	PUT_TEST(ft_putnbr_fd(0, p_[1]), "0");
	PUT_TEST(ft_putnbr_fd(7, p_[1]), "7");
	PUT_TEST(ft_putnbr_fd(-7, p_[1]), "-7");
	PUT_TEST(ft_putnbr_fd(10, p_[1]), "10");
	PUT_TEST(ft_putnbr_fd(-42, p_[1]), "-42");
	PUT_TEST(ft_putnbr_fd(100, p_[1]), "100");
	PUT_TEST(ft_putnbr_fd(123456789, p_[1]), "123456789");
	PUT_TEST(ft_putnbr_fd(INT_MAX, p_[1]), "2147483647");
	PUT_TEST(ft_putnbr_fd(INT_MIN, p_[1]), "-2147483648");
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
		"ft_lstnew(&x) : content doit valoir &x et next NULL");
	free(n);
	n = ft_lstnew(NULL);
	CHECK(n && n->content == NULL && n->next == NULL,
		"ft_lstnew(NULL) doit creer un noeud avec content = NULL et next = NULL");
	free(n);
}

static void	t_lstadd_size_last(void)
{
	t_list	*lst;
	t_list	*a;
	t_list	*b;
	t_list	*c;

	lst = NULL;
	CHECK((long)ft_lstsize(NULL) == 0, "ft_lstsize(NULL) = %ld, attendu 0",
		(long)ft_lstsize(NULL));
	CHECK(ft_lstlast(NULL) == NULL, "ft_lstlast(NULL) doit valoir NULL");
	a = ft_lstnew("a");
	b = ft_lstnew("b");
	c = ft_lstnew("c");
	ft_lstadd_back(&lst, b);
	CHECK(lst == b, "ft_lstadd_back(&lst, b) sur une liste vide : lst doit devenir b");
	CHECK((long)ft_lstsize(lst) == 1, "ft_lstsize d'une liste d'un noeud = %ld, attendu 1",
		(long)ft_lstsize(lst));
	CHECK(ft_lstlast(lst) == b, "ft_lstlast d'une liste d'un noeud doit renvoyer ce noeud");
	ft_lstadd_front(&lst, a);
	CHECK(lst == a && a->next == b, "ft_lstadd_front(&lst, a) : a doit devenir le premier, suivi de b");
	ft_lstadd_back(&lst, c);
	CHECK(b->next == c && c->next == NULL, "ft_lstadd_back(&lst, c) : c doit etre ajoute a la fin");
	CHECK((long)ft_lstsize(lst) == 3, "ft_lstsize = %ld, attendu 3", (long)ft_lstsize(lst));
	CHECK(ft_lstlast(lst) == c, "ft_lstlast doit renvoyer le dernier noeud (c)");
	CHECK(lst == a && a->next == b, "l'ordre de la liste doit etre a -> b -> c");
	free(a);
	free(b);
	free(c);
}

static void	t_lst_long(void)
{
	t_list	*lst;
	t_list	*last;
	int		i;
	int		ok;
	t_list	*cur;

	lst = NULL;
	last = NULL;
	i = 0;
	while (i < 1000)
	{
		last = ft_lstnew(strdup("noeud"));
		if (i % 2)
			ft_lstadd_back(&lst, last);
		else
			ft_lstadd_front(&lst, last);
		i++;
	}
	CHECK((long)ft_lstsize(lst) == 1000, "ft_lstsize d'une liste de 1000 noeuds = %ld",
		(long)ft_lstsize(lst));
	ok = 1;
	cur = lst;
	while (cur && cur->next)
		cur = cur->next;
	if (ft_lstlast(lst) != cur)
		ok = 0;
	CHECK(ok, "ft_lstlast d'une liste de 1000 noeuds");
	g_del_count = 0;
	ft_lstclear(&lst, del_count);
	CHECK(g_del_count == 1000 && lst == NULL,
		"ft_lstclear sur 1000 noeuds : del appele %d fois (attendu 1000), liste %s",
		g_del_count, lst ? "pas NULL" : "NULL");
}

static void	t_lstdelone_clear(void)
{
	t_list	*lst;
	t_list	*n;
	t_list	*keep;

	g_del_count = 0;
	n = ft_lstnew(strdup("seul"));
	ft_lstdelone(n, del_count);
	CHECK(g_del_count == 1, "ft_lstdelone doit appeler del une fois (appele %d fois)",
		g_del_count);
	g_del_count = 0;
	keep = ft_lstnew(strdup("suivant"));
	n = ft_lstnew(strdup("premier"));
	n->next = keep;
	ft_lstdelone(n, del_count);
	CHECK(g_del_count == 1 && strcmp(keep->content, "suivant") == 0,
		"ft_lstdelone ne doit pas liberer le noeud suivant");
	free(keep->content);
	free(keep);
	lst = NULL;
	ft_lstadd_back(&lst, ft_lstnew(strdup("1")));
	ft_lstadd_back(&lst, ft_lstnew(strdup("2")));
	ft_lstadd_back(&lst, ft_lstnew(strdup("3")));
	g_del_count = 0;
	ft_lstclear(&lst, del_count);
	CHECK(g_del_count == 3, "ft_lstclear : del appele %d fois, attendu 3", g_del_count);
	CHECK(lst == NULL, "ft_lstclear doit mettre la liste a NULL");
	ft_lstclear(&lst, del_count);
	CHECK(lst == NULL, "ft_lstclear sur une liste vide ne doit pas planter");
}

static void	t_lstiter_map(void)
{
	t_list	*lst;
	t_list	*m;
	char	s1[] = "un";
	char	s2[] = "deux";
	char	s3[] = "trois";

	lst = NULL;
	ft_lstadd_back(&lst, ft_lstnew(s1));
	ft_lstadd_back(&lst, ft_lstnew(s2));
	ft_lstadd_back(&lst, ft_lstnew(s3));
	m = ft_lstmap(lst, map_dup_upper, free);
	CHECK(m && (long)ft_lstsize(m) == 3 && strcmp(m->content, "UN") == 0
		&& strcmp(m->next->content, "DEUX") == 0
		&& strcmp(m->next->next->content, "TROIS") == 0,
		"ft_lstmap([un, deux, trois], majuscules) doit donner [UN, DEUX, TROIS] dans l'ordre");
	CHECK(m != lst && strcmp(s1, "un") == 0,
		"ft_lstmap doit creer une nouvelle liste sans modifier l'ancienne");
	ft_lstclear(&m, free);
	ft_lstiter(lst, iter_upper);
	CHECK(strcmp(s1, "UN") == 0 && strcmp(s2, "DEUX") == 0 && strcmp(s3, "TROIS") == 0,
		"ft_lstiter(liste, majuscules) doit etre applique a tous les noeuds (obtenu %s, %s, %s)",
		s1, s2, s3);
	CHECK(ft_lstmap(NULL, map_dup_upper, free) == NULL, "ft_lstmap(NULL, f, del) doit renvoyer NULL");
	ft_lstiter(NULL, iter_upper);
	CHECK(1, "ft_lstiter(NULL, f) ne plante pas");
	free(lst->next->next);
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
	run_test("listes de 1000 noeuds", t_lst_long);
	run_test("ft_lstdelone / ft_lstclear", t_lstdelone_clear);
	run_test("ft_lstiter / ft_lstmap", t_lstiter_map);
	return (print_summary());
}
