/*
** test_printf.c - testeur de ft_printf
** Chaque appel est compare au vrai printf : meme affichage, meme retour.
** Lance-le avec ./run.sh
*/

#include "../common/tester.h"
#include "ft_printf.h"
#include <limits.h>
#include <stdint.h>
#include <fcntl.h>

/* redirige la sortie standard vers un fichier temporaire */
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

/* remet la sortie standard et recupere ce qui a ete ecrit */
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

/*
** PF(format, ...) : appelle printf puis ft_printf avec les memes
** arguments, et compare l'affichage et la valeur de retour.
*/
#define PF(...) \
	do { \
		char	e_[4096]; \
		char	g_[4096]; \
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
			"ft_printf(%s)\n          attendu %s (retour %d)\n          obtenu  %s (retour %d)", \
			#__VA_ARGS__, show(e_, le_, se_), re_, show(g_, lg_, sg_), rg_); \
	} while (0)

static void	t_text(void)
{
	PF("bonjour");
	PF("");
	PF("avec un retour a la ligne\n");
	PF("\t tabulation et espaces   ");
}

static void	t_c(void)
{
	PF("%c", 'a');
	PF("%c", '0');
	PF("%c%c%c", 'a', 'b', 'c');
	PF(" %c %c ", 'x', 'y');
	PF("%c", '\0');
	PF("avant %c apres", '\0');
	PF("%c", 127);
}

static void	t_s(void)
{
	char	*null_str;

	null_str = NULL;
	PF("%s", "bonjour");
	PF("%s", "");
	PF("[%s] [%s]", "un", "deux");
	PF("%s", "avec\nretour");
	PF("%s", null_str);
	PF("debut %s fin", null_str);
}

static void	t_p(void)
{
	int		x;
	char	*heap;

	heap = malloc(1);
	PF("%p", (void *)&x);
	PF("%p", (void *)heap);
	PF("%p", NULL);
	PF("%p", (void *)1);
	PF("%p", (void *)15);
	PF("%p", (void *)16);
	PF("%p", (void *)ULONG_MAX);
	PF("%p", (void *)LONG_MIN);
	PF("%p %p", NULL, (void *)&x);
	free(heap);
}

static void	t_d_i(void)
{
	PF("%d", 0);
	PF("%d", 42);
	PF("%d", -42);
	PF("%d", 9);
	PF("%d", 10);
	PF("%d", -10);
	PF("%d", INT_MAX);
	PF("%d", INT_MIN);
	PF("%i", 0);
	PF("%i", -1);
	PF("%i", INT_MAX);
	PF("%i", INT_MIN);
	PF("%d%i%d", 1, -2, 3);
}

static void	t_u(void)
{
	PF("%u", 0);
	PF("%u", 42);
	PF("%u", UINT_MAX);
	PF("%u", -1);
	PF("%u", INT_MIN);
	PF("%u", 4000000000U);
}

static void	t_x(void)
{
	PF("%x", 0);
	PF("%x", 9);
	PF("%x", 10);
	PF("%x", 15);
	PF("%x", 16);
	PF("%x", 255);
	PF("%x", 4096);
	PF("%x", UINT_MAX);
	PF("%x", -1);
	PF("%x", INT_MIN);
	PF("%X", 0);
	PF("%X", 255);
	PF("%X", 0xABCDEF);
	PF("%X", UINT_MAX);
	PF("%X", -42);
}

static void	t_percent(void)
{
	PF("%%");
	PF("100%%");
	PF("%%%%");
	PF("%%d %d", 5);
}

static void	t_mix(void)
{
	int	x;

	PF("%s a %d ans, %x en hexa\n", "Nico", 20, 20);
	PF("%c|%s|%p|%d|%i|%u|%x|%X|%%", 'A', "str", (void *)&x, -1, 2, 3U, 255, 255);
	PF("%d %s %c %s", 0, "", 'z', (char *)NULL);
	PF("%x%X%u", 0, 0, 0);
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
		"ft_printf(NULL) doit renvoyer -1 sans rien afficher (obtenu : retour %d, %zu caracteres)",
		ret, len);
}

int	main(void)
{
	print_title("ft_printf - conversions");
	run_test("texte sans conversion", t_text);
	run_test("%c", t_c);
	run_test("%s", t_s);
	run_test("%p", t_p);
	run_test("%d et %i", t_d_i);
	run_test("%u", t_u);
	run_test("%x et %X", t_x);
	run_test("%%", t_percent);
	run_test("plusieurs conversions melangees", t_mix);
	print_title("ft_printf - cas limite (comme le printf de Linux)");
	run_test("ft_printf(NULL) renvoie -1", t_null_format);
	return (print_summary());
}
