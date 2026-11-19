/*
** tester.h - socle commun des testeurs
**
** Chaque test est lance dans un processus enfant (fork) :
**  - un crash (segfault...) n'arrete pas les autres tests ;
**  - une boucle infinie est coupee au bout de TEST_TIMEOUT secondes ;
**  - les details des erreurs sont affiches sous le nom du test.
*/

#ifndef TESTER_H
# define TESTER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
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

/*
** CHECK(condition, "message", ...) : si la condition est fausse,
** le test est KO et le message est affiche (10 messages max par test).
*/
# define CHECK(cond, ...) \
	do { \
		if (!(cond)) \
		{ \
			g_fail = 1; \
			if (g_nb_msg++ < 10) \
			{ \
				fprintf(stderr, "      - "); \
				fprintf(stderr, __VA_ARGS__); \
				fprintf(stderr, "\n"); \
			} \
		} \
	} while (0)

__attribute__((unused))
static void	print_title(const char *title)
{
	printf("\n" BOLD "=== %s ===" RESET "\n", title);
	fflush(stdout);
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
		alarm(TEST_TIMEOUT);
		fn();
		fflush(stderr);
		exit(g_fail ? 1 : 0);
	}
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	{
		printf("  " GREEN "[OK]" RESET "    %s\n", name);
		g_ok++;
	}
	else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
	{
		printf("  " RED "[TIMEOUT]" RESET " %s (boucle infinie ?)\n", name);
		g_ko++;
	}
	else if (WIFSIGNALED(status))
	{
		printf("  " RED "[CRASH]" RESET " %s (%s)\n", name,
			strsignal(WTERMSIG(status)));
		g_ko++;
	}
	else if (WIFEXITED(status) && WEXITSTATUS(status) == 42)
	{
		printf("  " RED "[LEAK]" RESET "  %s (erreur memoire / fuite)\n", name);
		g_ko++;
	}
	else
	{
		printf("  " RED "[KO]" RESET "    %s\n", name);
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
		j += sprintf(out + j, "...");
	out[j++] = '"';
	out[j] = '\0';
	return (out);
}

#endif
