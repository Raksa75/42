#ifndef TESTER_H
# define TESTER_H

# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdarg.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>
# include <sys/mman.h>

# define GREEN "\033[32m"
# define RED "\033[31m"
# define YELLOW "\033[33m"
# define CYAN "\033[36m"
# define BOLD "\033[1m"
# define DIM "\033[2m"
# define RESET "\033[0m"
# define ICON_OK GREEN "✔" RESET
# define ICON_KO RED "✘" RESET

# ifndef TEST_TIMEOUT
#  define TEST_TIMEOUT 30
# endif

static int	g_ok = 0;
static int	g_ko = 0;
static int	g_fail = 0;
static int	g_nb_msg = 0;
static long	g_checks_ok = 0;
static long	g_checks = 0;

<<<<<<< HEAD
static long	*g_shared = NULL;
=======
static int	g_crash = 0;
static long	*g_shared = NULL;
static char	g_skipped[8192] = "";
static char	g_pending_title[256] = "";
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)

__attribute__((unused))
static void	log_printf(const char *fmt, ...)
{
	static FILE	*f = NULL;
	static int	tried = 0;
	const char	*path;
	va_list		ap;

	if (!tried)
	{
		tried = 1;
		path = getenv("LOG_FILE");
		if (path && *path)
			f = fopen(path, "a");
	}
	if (!f)
		return ;
	va_start(ap, fmt);
	vfprintf(f, fmt, ap);
	va_end(ap);
	fflush(f);
}

__attribute__((unused))
static int	compact(void)
{
	const char	*c;

	c = getenv("TESTER_COMPACT");
	return (c && *c == '1');
}

__attribute__((unused))
static void	check_result(int ok, const char *msg)
{
	if (g_shared)
	{
		g_shared[1]++;
		if (ok)
			g_shared[0]++;
	}
	if (ok)
	{
		log_printf("      " ICON_OK " %s\n", msg);
		return ;
	}
	log_printf("      " ICON_KO " " RED "%s" RESET "\n", msg);
	g_fail = 1;
	if (g_nb_msg++ < (compact() ? 3 : 10))
		fprintf(stderr, "%s" RED "↳" RESET " %s\n",
			compact() ? "            " : "        ", msg);
	else if (g_nb_msg == (compact() ? 4 : 11))
		fprintf(stderr, "%s" DIM "↳ ... (les autres erreurs sont dans le log)"
			RESET "\n", compact() ? "            " : "        ");
}

# define CHECK(cond, ...) \
	do { \
		char	m_[4096]; \
		int		ok_; \
		ok_ = (cond) ? 1 : 0; \
		snprintf(m_, sizeof(m_), __VA_ARGS__); \
		check_result(ok_, m_); \
	} while (0)

__attribute__((unused))
static void	print_bar(FILE *f, long ok, long total, int width)
{
	int	full;
	int	i;

	full = (total > 0) ? (int)(ok * width / total) : width;
	if (ok < total && full == width)
		full--;
	fprintf(f, GREEN);
	i = 0;
	while (i < width)
	{
		if (i == full)
			fprintf(f, RED);
		fprintf(f, "%s", i < full ? "█" : "░");
		i++;
	}
	fprintf(f, RESET);
}

__attribute__((unused))
static void	print_title(const char *title)
{
	log_printf("\n" CYAN BOLD "━━ %s ━━" RESET "\n", title);
	if (compact())
		return ;
<<<<<<< HEAD
=======
	if (getenv("TEST_FILTER") && *getenv("TEST_FILTER"))
	{
		snprintf(g_pending_title, sizeof(g_pending_title), "%s", title);
		return ;
	}
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
	printf("\n " CYAN BOLD "━━ %s " RESET DIM, title);
	printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
		+ 3 * (strlen(title) < 50 ? strlen(title) : 50));
	printf(RESET "\n");
	fflush(stdout);
}

__attribute__((unused))
static void	print_test_line(const char *name, const char *icon,
	const char *tag, long *count)
{
	if (compact())
		printf("       %s %-46s " DIM "%4ld/%-4ld" RESET, icon, name, count[0],
			count[1]);
	else
		printf("   %s %-50s " DIM "%4ld/%-4ld" RESET, icon, name, count[0],
			count[1]);
	if (tag)
		printf("  " RED BOLD "%s" RESET, tag);
	printf("\n");
}

__attribute__((unused))
static int	report(const char *name, int status, long *count)
{
	char	tag[128];
	int		ok;

	ok = 0;
	tag[0] = '\0';
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		ok = 1;
	else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
		snprintf(tag, sizeof(tag), "TIMEOUT (boucle infinie ?)");
	else if (WIFSIGNALED(status))
		snprintf(tag, sizeof(tag), "CRASH (%s)", strsignal(WTERMSIG(status)));
	else if (WIFEXITED(status) && WEXITSTATUS(status) == 42)
		snprintf(tag, sizeof(tag), "LEAK (fuite / erreur memoire)");
	if (ok && !compact())
		print_test_line(name, ICON_OK, NULL, count);
	else if (!ok)
		print_test_line(name, ICON_KO, tag[0] ? tag : NULL, count);
	if (ok)
		log_printf("    " GREEN BOLD "→ OK" RESET DIM " (%ld/%ld verifications)"
			RESET "\n", count[0], count[1]);
	else if (tag[0])
		log_printf("    " RED BOLD "→ %s" RESET " : le programme s'est arrete juste"
			" apres la derniere verification ci-dessus (%ld/%ld OK avant)\n",
			tag, count[0], count[1]);
	else
		log_printf("    " RED BOLD "→ KO" RESET " (%ld/%ld verifications OK,"
			" voir les lignes ✘ ci-dessus)\n", count[0], count[1]);
<<<<<<< HEAD
=======
	if (!ok && tag[0])
		return (2);
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
	return (ok);
}

__attribute__((unused))
<<<<<<< HEAD
=======
static void	save_state(const char *name, int ok)
{
	const char	*path;
	const char	*prefix;
	FILE		*f;

	path = getenv("STATE_FILE");
	if (!path || !*path)
		return ;
	f = fopen(path, "a");
	if (!f)
		return ;
	prefix = getenv("TEST_PREFIX");
	fprintf(f, "%s\ttest : %s%s\n", ok ? "OK" : "KO", prefix ? prefix : "", name);
	fclose(f);
}

__attribute__((unused))
static void	strip_ft(const char *src, char *dst, size_t size)
{
	size_t	j;

	j = 0;
	while (*src && j + 1 < size)
	{
		if (strncasecmp(src, "ft_", 3) == 0)
			src += 3;
		else
			dst[j++] = *src++;
	}
	dst[j] = '\0';
}

__attribute__((unused))
static int	filtered_out(const char *name)
{
	const char	*filter;
	char		f[256];
	char		n[512];
	size_t		len;

	filter = getenv("TEST_FILTER");
	if (!filter || !*filter)
		return (0);
	strip_ft(filter, f, sizeof(f));
	strip_ft(name, n, sizeof(n));
	if (strcasestr(n, f))
	{
		if (g_pending_title[0] && !compact())
			printf("\n " CYAN BOLD "━━ %s" RESET "\n", g_pending_title);
		g_pending_title[0] = '\0';
		return (0);
	}
	len = strlen(g_skipped);
	if (len + strlen(name) + 16 < sizeof(g_skipped))
		snprintf(g_skipped + len, sizeof(g_skipped) - len, "      • %s\n", name);
	return (1);
}

__attribute__((unused))
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
static void	run_test(const char *name, void (*fn)(void))
{
	int		pipefd[2];
	pid_t	pid;
	int		status;
	char	buf[4096];
	ssize_t	n;

	int		res;

	if (filtered_out(name))
		return ;
	fflush(stdout);
	fflush(stderr);
	log_printf("\n  " BOLD "▸ %s" RESET "\n", name);
	if (!g_shared)
		g_shared = mmap(NULL, 2 * sizeof(long), PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (g_shared == MAP_FAILED || pipe(pipefd) == -1)
		return ;
	g_shared[0] = 0;
	g_shared[1] = 0;
	pid = fork();
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 2);
		close(pipefd[1]);
		g_fail = 0;
		g_nb_msg = 0;
		alarm(TEST_TIMEOUT);
		fn();
		fflush(stderr);
		exit(g_fail ? 1 : 0);
	}
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	g_checks_ok += g_shared[0];
	g_checks += g_shared[1];
<<<<<<< HEAD
	if (report(name, status, g_shared))
		g_ok++;
	else
		g_ko++;
=======
	res = report(name, status, g_shared);
	save_state(name, res == 1);
	if (res == 1)
		g_ok++;
	else
		g_ko++;
	if (res == 2)
		g_crash++;
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
	fflush(stdout);
	n = read(pipefd[0], buf, sizeof(buf) - 1);
	while (n > 0)
	{
		buf[n] = '\0';
		printf("%s", buf);
		n = read(pipefd[0], buf, sizeof(buf) - 1);
	}
	close(pipefd[0]);
	fflush(stdout);
}

__attribute__((unused))
static int	print_summary(void)
{
	const char	*path;
	FILE		*f;

	path = getenv("RESULT_FILE");
	f = (path && *path) ? fopen(path, "w") : NULL;
	if (f)
	{
<<<<<<< HEAD
		fprintf(f, "%d %d %ld %ld\n", g_ok, g_ok + g_ko, g_checks_ok, g_checks);
		fclose(f);
	}
=======
		fprintf(f, "%d %d %ld %ld %d\n", g_ok, g_ok + g_ko, g_checks_ok, g_checks,
			g_crash);
		fclose(f);
	}
	if (g_ok + g_ko == 0 && getenv("TEST_FILTER") && !compact())
	{
		printf("\n   " YELLOW "!" RESET " aucun test ne correspond a \"%s\"."
			" Les tests de ce projet :\n%s", getenv("TEST_FILTER"), g_skipped);
		fflush(stdout);
		return (0);
	}
>>>>>>> 851f1b2 (MAJ testeur, ajout de test sur fonctions precise, check norme et valgrind avec raksa exam, check si fichier en trop (.o ..etc), check de header)
	log_printf("\n  %s " BOLD "%d/%d tests OK" RESET " (%ld/%ld verifications)\n",
		g_ko ? ICON_KO : ICON_OK, g_ok, g_ok + g_ko, g_checks_ok, g_checks);
	if (compact())
		return (g_ko != 0);
	printf("\n   %s ", g_ko ? ICON_KO : ICON_OK);
	print_bar(stdout, g_ok, g_ok + g_ko, 30);
	printf("  %s%d/%d tests OK" RESET DIM "  (%ld/%ld verifications)" RESET "\n",
		g_ko ? RED BOLD : GREEN BOLD, g_ok, g_ok + g_ko, g_checks_ok, g_checks);
	fflush(stdout);
	return (g_ko != 0);
}

__attribute__((unused))
static char	*show(const char *s, size_t len, char *out)
{
	size_t	i;
	size_t	j;

	if (!s)
		return (strcpy(out, "NULL"));
	j = 0;
	out[j++] = '"';
	i = 0;
	while (i < len && i < 60)
	{
		if (s[i] == '\n')
			j += sprintf(out + j, "\\n");
		else if (s[i] == '\0')
			j += sprintf(out + j, "\\0");
		else if ((unsigned char)s[i] < 32 || (unsigned char)s[i] > 126)
			j += sprintf(out + j, "\\x%02x", (unsigned char)s[i]);
		else
			out[j++] = s[i];
		i++;
	}
	if (i < len)
		j += sprintf(out + j, "...\" (%zu caracteres)", len);
	else
		out[j++] = '"';
	out[j] = '\0';
	return (out);
}

#endif
