/*
** tester.h - socle commun des testeurs
**
** Chaque test est lance dans un processus enfant (fork) :
**  - un crash (segfault...) n'arrete pas les autres tests ;
**  - une boucle infinie est coupee au bout de TEST_TIMEOUT secondes ;
**  - les details des erreurs sont affiches sous le nom du test.
**
** Si la variable d'environnement LOG_FILE est definie, chaque
** verification (reussie ou non) y est ecrite avec ses valeurs.
*/

#ifndef TESTER_H
# define TESTER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdarg.h>
# include <unistd.h>
# include <signal.h>
# include <sys/wait.h>

# define GREEN "\033[32m"
# define RED "\033[31m"
# define YELLOW "\033[33m"
# define BOLD "\033[1m"
# define RESET "\033[0m"

# ifndef TEST_TIMEOUT
#  define TEST_TIMEOUT 30
# endif

static int	g_ok = 0;
static int	g_ko = 0;
static int	g_fail = 0;
static int	g_nb_msg = 0;
static int	g_nb_checks = 0;

/* ------------------------------ le log ------------------------------ */

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

/* ---------------------------- verifications ---------------------------- */

__attribute__((unused))
static void	check_result(int ok, const char *msg)
{
	g_nb_checks++;
	log_printf("      [%s] %s\n", ok ? "OK" : "KO", msg);
	if (ok)
		return ;
	g_fail = 1;
	if (g_nb_msg++ < 10)
		fprintf(stderr, "      - %s\n", msg);
	else if (g_nb_msg == 11)
		fprintf(stderr, "      - ... (autres erreurs dans le log)\n");
}

/*
** CHECK(condition, "description", ...) : la description est ecrite dans
** le log dans tous les cas, et affichee a l'ecran si la condition est
** fausse. Elle doit dire ce qui est teste, et les valeurs obtenue /
** attendue.
*/
# define CHECK(cond, ...) \
	do { \
		char	m_[4096]; \
		int		ok_; \
		ok_ = (cond) ? 1 : 0; \
		snprintf(m_, sizeof(m_), __VA_ARGS__); \
		check_result(ok_, m_); \
	} while (0)

/* ------------------------------ affichage ------------------------------ */

__attribute__((unused))
static void	print_title(const char *title)
{
	printf("\n" BOLD "=== %s ===" RESET "\n", title);
	fflush(stdout);
	log_printf("\n=== %s ===\n", title);
}

__attribute__((unused))
static void	run_test(const char *name, void (*fn)(void))
{
	int		pipefd[2];
	pid_t	pid;
	int		status;
	char	buf[4096];
	ssize_t	n;

	fflush(stdout);
	fflush(stderr);
	log_printf("\n  # %s\n", name);
	if (pipe(pipefd) == -1)
		return ;
	pid = fork();
	if (pid == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], 2);
		close(pipefd[1]);
		g_fail = 0;
		g_nb_msg = 0;
		g_nb_checks = 0;
		alarm(TEST_TIMEOUT);
		fn();
		log_printf("      (%d verifications)\n", g_nb_checks);
		fflush(stderr);
		exit(g_fail ? 1 : 0);
	}
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		printf("  " GREEN "[OK]" RESET "      %s\n", name);
		log_printf("    => OK\n");
		g_ok++;
	}
	else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
	{
		printf("  " RED "[TIMEOUT]" RESET " %s (boucle infinie ?)\n", name);
		log_printf("    => TIMEOUT : le test a depasse %d secondes (boucle infinie ?)."
			" Il bloquait apres la derniere verification ci-dessus.\n",
			TEST_TIMEOUT);
		g_ko++;
	}
	else if (WIFSIGNALED(status))
	{
		printf("  " RED "[CRASH]" RESET "   %s (%s)\n", name,
			strsignal(WTERMSIG(status)));
		log_printf("    => CRASH (%s) : le programme a plante juste apres la"
			" derniere verification ci-dessus.\n", strsignal(WTERMSIG(status)));
		g_ko++;
	}
	else if (WIFEXITED(status) && WEXITSTATUS(status) == 42)
	{
		printf("  " RED "[LEAK]" RESET "    %s (erreur memoire / fuite)\n", name);
		log_printf("    => LEAK : valgrind a trouve une fuite ou une erreur memoire\n");
		g_ko++;
	}
	else
	{
		printf("  " RED "[KO]" RESET "      %s\n", name);
		log_printf("    => KO (voir les lignes [KO] ci-dessus)\n");
		g_ko++;
	}
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
	printf("\n" BOLD "Resultat : " RESET);
	if (g_ko == 0)
		printf(GREEN "%d/%d tests OK" RESET "\n", g_ok, g_ok + g_ko);
	else
		printf(RED "%d/%d tests OK, %d KO" RESET "\n", g_ok, g_ok + g_ko,
			g_ko);
	log_printf("\nResultat : %d/%d groupes de tests OK\n", g_ok, g_ok + g_ko);
	return (g_ko != 0);
}

/*
** Affiche une chaine en rendant visibles les caracteres speciaux
** (\n, \0, ...). Le resultat est ecrit dans out (taille 1024).
*/
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
