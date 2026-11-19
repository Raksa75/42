/*
** test_gnl.c - testeur de get_next_line
** Compile une fois par BUFFER_SIZE par ./run.sh
*/

#include "../common/tester.h"
#include "get_next_line.h"
#include <fcntl.h>

static char			g_dir[] = "/tmp/gnl_testXXXXXX";
static const char	*g_content;
static size_t		g_len;

/* ecrit le contenu de test dans un fichier et renvoie son chemin */
static char	*make_file(const char *content, size_t len)
{
	static char	path[256];
	static int	count = 0;
	int			fd;

	sprintf(path, "%s/f%d.txt", g_dir, count++);
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return (NULL);
	if (len > 0)
		write(fd, content, len);
	close(fd);
	return (path);
}

/*
** Lit tout le fd avec get_next_line et compare chaque ligne a ce qui
** est attendu (le contenu decoupe apres chaque '\n').
*/
static void	check_fd(int fd, const char *content, size_t len)
{
	size_t	start;
	size_t	end;
	char	*line;
	int		num;
	char	b1[1024];
	char	b2[1024];

	start = 0;
	num = 1;
	while (start < len)
	{
		end = start;
		while (end < len && content[end] != '\n')
			end++;
		if (end < len)
			end++;
		line = get_next_line(fd);
		CHECK(line && strlen(line) == end - start
			&& memcmp(line, content + start, end - start) == 0,
			"ligne %d : obtenu %s, attendu %s", num,
			show(line, line ? strlen(line) : 0, b1),
			show(content + start, end - start, b2));
		free(line);
		start = end;
		num++;
	}
	line = get_next_line(fd);
	CHECK(line == NULL, "a la fin du fichier, get_next_line doit renvoyer NULL (obtenu %s)",
		show(line, line ? strlen(line) : 0, b1));
	free(line);
	line = get_next_line(fd);
	CHECK(line == NULL, "un nouvel appel apres la fin doit encore renvoyer NULL");
	free(line);
}

static void	t_file(void)
{
	char	*path;
	int		fd;

	path = make_file(g_content, g_len);
	fd = open(path, O_RDONLY);
	CHECK(fd != -1, "impossible de creer le fichier de test");
	if (fd == -1)
		return ;
	check_fd(fd, g_content, g_len);
	close(fd);
}

static void	file_test(const char *name, const char *content, size_t len)
{
	g_content = content;
	g_len = len;
	run_test(name, t_file);
}

static void	t_stdin(void)
{
	const char	*content = "ligne 1 depuis stdin\nligne 2\n\nderniere";
	int			p[2];
	pid_t		pid;

	if (pipe(p) == -1)
		return ;
	pid = fork();
	if (pid == 0)
	{
		close(p[0]);
		write(p[1], content, strlen(content));
		close(p[1]);
		exit(0);
	}
	close(p[1]);
	dup2(p[0], 0);
	close(p[0]);
	check_fd(0, content, strlen(content));
	waitpid(pid, NULL, 0);
}

static void	t_invalid_fd(void)
{
	char	*line;

	line = get_next_line(-1);
	CHECK(line == NULL, "get_next_line(-1) doit renvoyer NULL");
	free(line);
	line = get_next_line(4242);
	CHECK(line == NULL, "get_next_line(4242), fd jamais ouvert, doit renvoyer NULL");
	free(line);
}

static void	t_two_files(void)
{
	const char	*a = "fichier A ligne 1\nfichier A ligne 2\n";
	const char	*b = "fichier B\nsans retour final";
	int			fd;

	fd = open(make_file(a, strlen(a)), O_RDONLY);
	check_fd(fd, a, strlen(a));
	close(fd);
	fd = open(make_file(b, strlen(b)), O_RDONLY);
	check_fd(fd, b, strlen(b));
	close(fd);
}

int	main(void)
{
	static char	long_line[10001];
	static char	long_then_short[5020];
	static char	many[20000];
	size_t		n;
	int			i;
	int			j;
	int			status;

	if (!mkdtemp(g_dir))
		return (1);
	memset(long_line, 'x', 10000);
	memset(long_then_short, 'y', 5000);
	strcpy(long_then_short + 5000, "\nfin\n");
	n = 0;
	i = 0;
	while (i < 300)
	{
		j = 0;
		while (j < i % 47)
			many[n++] = 'a' + (i + j++) % 26;
		many[n++] = '\n';
		i++;
	}
	printf("\n" BOLD "=== get_next_line - BUFFER_SIZE = %d ===" RESET "\n",
		(int)BUFFER_SIZE);
	file_test("fichier vide", "", sizeof("") - 1);
	file_test("une ligne sans '\\n' (\"a\")", "a", 1);
	file_test("une ligne avec '\\n' (\"a\\n\")", "a\n", 2);
	file_test("\"abcd\\nefgh\\nij\" (pas de '\\n' final)", "abcd\nefgh\nij", 12);
	file_test("\"abcd\\nefgh\\nij\\n\"", "abcd\nefgh\nij\n", 13);
	file_test("un seul '\\n'", "\n", sizeof("\n") - 1);
	file_test("que des '\\n'", "\n\n\n\n", sizeof("\n\n\n\n") - 1);
	file_test("lignes vides au milieu", "\nabc\n\n\ndef\n\nfin", sizeof("\nabc\n\n\ndef\n\nfin") - 1);
	file_test("une ligne de 10000 caracteres", long_line, 10000);
	file_test("ligne longue puis ligne courte", long_then_short, 5005);
	file_test("300 lignes de longueurs variees", many, n);
	run_test("lecture sur l'entree standard (fd 0)", t_stdin);
	run_test("fd invalides (-1 et 4242)", t_invalid_fd);
	run_test("deux fichiers l'un apres l'autre", t_two_files);
	status = print_summary();
	sprintf(long_line, "rm -rf %s", g_dir);
	system(long_line);
	return (status);
}
