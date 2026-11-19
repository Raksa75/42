#include "../common/tester.h"
#include "ft_printf.h"
#include <limits.h>
#include <stdint.h>
#include <fcntl.h>

static int	capture_start(int *saved)
{
	char	path[] = "/tmp/pf_testXXXXXX";
	int		fd;

	fd = mkstemp(path);
	unlink(path);
	fflush(stdout);
	*saved = dup(1);
	dup2(fd, 1);
	return (fd);
}

static size_t	capture_end(int fd, int saved, char *buf, size_t size)
{
	ssize_t	n;

	fflush(stdout);
	dup2(saved, 1);
	close(saved);
	lseek(fd, 0, SEEK_SET);
	n = read(fd, buf, size - 1);
	close(fd);
	if (n < 0)
		n = 0;
	buf[n] = '\0';
	return ((size_t)n);
}

#define PF(...) PFD(#__VA_ARGS__, __VA_ARGS__)
#define PFD(desc, ...) \
	do { \
		char	e_[8192]; \
		char	g_[8192]; \
		char	se_[1024]; \
		char	sg_[1024]; \
		int		s_; \
		int		f_; \
		int		re_; \
		int		rg_; \
		size_t	le_; \
		size_t	lg_; \
		f_ = capture_start(&s_); \
		re_ = printf(__VA_ARGS__); \
		le_ = capture_end(f_, s_, e_, sizeof(e_)); \
		f_ = capture_start(&s_); \
		rg_ = ft_printf(__VA_ARGS__); \
		lg_ = capture_end(f_, s_, g_, sizeof(g_)); \
		CHECK(re_ == rg_ && le_ == lg_ && memcmp(e_, g_, le_) == 0, \
			"ft_printf(%s)\n          obtenu  %s (retour %d)\n          attendu %s (retour %d)", \
			desc, show(g_, lg_, sg_), rg_, show(e_, le_, se_), re_); \
	} while (0)

static const int			g_ints[] = {0, 1, -1, 7, -7, 9, 10, -10, 11, 15, 16,
	17, 42, -42, 99, 100, -100, 255, 256, 999, 1000, 4095, 4096, 65535, 65536,
	123456, -123456, 1000000, 99999999, 100000000, 123456789, -123456789,
	999999999, 1000000000, -1000000000, 2147483646, INT_MAX, INT_MIN,
	INT_MIN + 1};
static const unsigned int	g_uints[] = {0, 1, 9, 10, 15, 16, 255, 256, 4095,
	4096, 65535, 65536, 1000000, 0x7fffffff, 0x80000000, 0xdeadbeef,
	4000000000U, UINT_MAX - 1, UINT_MAX};

#define NB_INTS (int)(sizeof(g_ints) / sizeof(*g_ints))
#define NB_UINTS (int)(sizeof(g_uints) / sizeof(*g_uints))

static void	t_text(void)
{
	PF("bonjour");
	PF("");
	PF("a");
	PF("avec un retour a la ligne\n");
	PF("\n");
	PF("\t tabulation et espaces   ");
	PF("plusieurs\nlignes\nde texte\n");
	PF("caracteres speciaux : \\ \" ' ? ! @ # $ ^ & * ( ) [ ] { }");
}

static void	t_c(void)
{
	char	d[128];
	int	c;

	c = 1;
	while (c < 128)
	{
		sprintf(d, "\"%%c\", %d", c);
		PFD(d, "%c", c);
		c++;
	}
	PF("%c", '\0');
	PF("avant %c apres", '\0');
	PF("%c%c%c", 'a', 'b', 'c');
	PF(" %c %c %c ", 'x', 'y', 'z');
	PF("%c%c", '\0', 'A');
	PF("%c", 200);
	PF("%c", 'A' + 256);
}

static void	t_s(void)
{
	char	*null_str;
	char	big[1001];

	null_str = NULL;
	memset(big, 'S', 1000);
	big[1000] = '\0';
	PF("%s", "bonjour");
	PF("%s", "");
	PF("%s", "a");
	PF("[%s] [%s]", "un", "deux");
	PF("%s%s%s", "a", "", "c");
	PF("%s", "avec\nretour");
	PF("%s", "100% sur");
	PF("%s", "%d n'est pas une conversion ici");
	PF("%s", "\t\v\f\r");
	PF("%s", big);
	PF("%s", null_str);
	PF("debut %s fin", null_str);
	PF("%s %s", null_str, "apres NULL");
}

static void	t_p(void)
{
	char	d[128];
	int		x;
	char	*heap;
	void	*ptrs[9];
	int		i;

	heap = malloc(1);
	ptrs[0] = &x;
	ptrs[1] = heap;
	ptrs[2] = (void *)1;
	ptrs[3] = (void *)15;
	ptrs[4] = (void *)16;
	ptrs[5] = (void *)0xabcdef;
	ptrs[6] = (void *)ULONG_MAX;
	ptrs[7] = (void *)LONG_MIN;
	ptrs[8] = (void *)LONG_MAX;
	i = 0;
	while (i < 9)
	{
		sprintf(d, "\"%%p\", (void *)%p", ptrs[i]);
		PFD(d, "%p", ptrs[i]);
		i++;
	}
	PF("%p", NULL);
	PF("%p %p", NULL, (void *)&x);
	PF("[%p]", (void *)t_text);
	PF("%p%p", (void *)heap, (void *)heap);
	free(heap);
}

static void	t_d(void)
{
	char	d[128];
	int	i;

	i = 0;
	while (i < NB_INTS)
	{
		sprintf(d, "\"%%d\", %d", g_ints[i]);
		PFD(d, "%d", g_ints[i]);
		i++;
	}
	PF("%d%d%d", 1, -2, 3);
	PF("[%d] [%d]", INT_MIN, INT_MAX);
	PF("%d", (int)'A');
}

static void	t_i(void)
{
	char	d[128];
	int	i;

	i = 0;
	while (i < NB_INTS)
	{
		sprintf(d, "\"%%i\", %d", g_ints[i]);
		PFD(d, "%i", g_ints[i]);
		i++;
	}
	PF("%i %d", -5, -5);
}

static void	t_u(void)
{
	char	d[128];
	int	i;

	i = 0;
	while (i < NB_UINTS)
	{
		sprintf(d, "\"%%u\", %uU", g_uints[i]);
		PFD(d, "%u", g_uints[i]);
		i++;
	}
	i = 0;
	while (i < NB_INTS)
	{
		sprintf(d, "\"%%u\", %d", g_ints[i]);
		PFD(d, "%u", g_ints[i]);
		i++;
	}
}

static void	t_x(void)
{
	char	d[128];
	int	i;

	i = 0;
	while (i < NB_UINTS)
	{
		sprintf(d, "\"%%x\", %uU", g_uints[i]);
		PFD(d, "%x", g_uints[i]);
		sprintf(d, "\"%%X\", %uU", g_uints[i]);
		PFD(d, "%X", g_uints[i]);
		i++;
	}
	i = 0;
	while (i < NB_INTS)
	{
		sprintf(d, "\"%%x\", %d", g_ints[i]);
		PFD(d, "%x", g_ints[i]);
		sprintf(d, "\"%%X\", %d", g_ints[i]);
		PFD(d, "%X", g_ints[i]);
		i++;
	}
	PF("%x%X", 0xabc, 0xabc);
}

static void	t_percent(void)
{
	PF("%%");
	PF("100%%");
	PF("%%%%");
	PF("%% %% %%");
	PF("%%d %d", 5);
	PF("%%s%s", "x");
	PF("%%%c%%", 'a');
}

static void	t_mix(void)
{
	int	x;

	PF("%s a %d ans, %x en hexa\n", "Nico", 20, 20);
	PF("%c|%s|%p|%d|%i|%u|%x|%X|%%", 'A', "str", (void *)&x, -1, 2, 3U, 255, 255);
	PF("%d %s %c %s", 0, "", 'z', (char *)NULL);
	PF("%x%X%u", 0, 0, 0);
	PF("%c%s%d%i%u%x%X%p%%", 'z', "s", 1, 2, 3U, 4U, 5U, (void *)6);
	PF("%d %d %d %d %d %d %d %d %d %d", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	PF("%s%s%s%s%s%s", "a", "b", "c", "d", "e", "f");
	PF("debut %d milieu %s fin", 42, "texte");
	PF("%d\n%s\n%c\n", -1, "lignes", 'x');
	PF("%u%%%x", 100U, 100U);
}

static void	t_return(void)
{
	char	big[1001];
	int		saved;
	int		fd;
	int		r;
	char	buf[8192];

	memset(big, 'r', 1000);
	big[1000] = '\0';
	fd = capture_start(&saved);
	r = ft_printf("%s%s", big, big);
	capture_end(fd, saved, buf, sizeof(buf));
	CHECK(r == 2000, "ft_printf(\"%%s%%s\", 1000 car., 1000 car.) : retour %d, attendu 2000", r);
	fd = capture_start(&saved);
	r = ft_printf("%c%c%c", '\0', '\0', '\0');
	capture_end(fd, saved, buf, sizeof(buf));
	CHECK(r == 3, "ft_printf(\"%%c%%c%%c\", 0, 0, 0) : retour %d, attendu 3 (les '\\0' comptent)", r);
	fd = capture_start(&saved);
	r = ft_printf("");
	capture_end(fd, saved, buf, sizeof(buf));
	CHECK(r == 0, "ft_printf(\"\") : retour %d, attendu 0", r);
}

static void	t_null_format(void)
{
	char	*fmt;
	int		saved;
	int		fd;
	int		ret;
	char	buf[64];
	size_t	len;

	fmt = NULL;
	fd = capture_start(&saved);
	ret = ft_printf(fmt);
	len = capture_end(fd, saved, buf, sizeof(buf));
	CHECK(ret == -1 && len == 0,
		"ft_printf(NULL) : retour %d et %zu caracteres affiches, attendu -1 et rien d'affiche (comme le printf de Linux)",
		ret, len);
}

int	main(void)
{
	print_title("ft_printf - conversions");
	run_test("texte sans conversion", t_text);
	run_test("%c (tous les caracteres ASCII, '\\0')", t_c);
	run_test("%s (chaines vides, longues, NULL)", t_s);
	run_test("%p (adresses, NULL)", t_p);
	run_test("%d (39 valeurs, INT_MIN, INT_MAX)", t_d);
	run_test("%i", t_i);
	run_test("%u (non signes, negatifs convertis)", t_u);
	run_test("%x et %X", t_x);
	run_test("%%", t_percent);
	run_test("plusieurs conversions melangees", t_mix);
	run_test("valeurs de retour", t_return);
	print_title("ft_printf - cas limite (comme le printf de Linux)");
	run_test("ft_printf(NULL) renvoie -1", t_null_format);
	return (print_summary());
}
