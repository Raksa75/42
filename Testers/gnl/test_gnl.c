/*
** test_gnl.c - testeur de get_next_line
** Compile une fois par BUFFER_SIZE par ./run.sh
** Chaque verification est ecrite dans le log (variable LOG_FILE).
*/

#include "../common/tester.h"
#include "get_next_line.h"
#include <fcntl.h>
#include <time.h>

static char			g_dir[] = "/tmp/gnl_testXXXXXX";
static const char	*g_content;
static size_t		g_len;
static char			g_cmd[256];

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
	if (len > 0 && write(fd, content, len) != (ssize_t)len)
		fprintf(stderr, "ecriture du fichier de test incomplete\n");
	close(fd);
	return (path);
}

static int	count_lines(const char *content, size_t len)
{
	size_t	i;
	int		n;

	n = 0;
	i = 0;
	while (i < len)
	{
		if (content[i] == '\n' || i == len - 1)
			n++;
		i++;
	}
	return (n);
}

/*
** Lit tout le fd avec get_next_line et compare chaque ligne a ce qui est
** attendu (le contenu decoupe apres chaque '\n'). Pour les petits
** fichiers, chaque ligne est ecrite dans le log ; pour les gros, une
** seule ligne de resume (avec la premiere erreur s'il y en a une).
*/
static void	check_fd(int fd, const char *content, size_t len)
{
	size_t	start;
	size_t	end;
	char	*line;
	int		num;
	int		nb_ok;
	int		verbose;
	int		first_err;
	char	b1[1024];
	char	b2[1024];
	char	err[2600];

	verbose = (count_lines(content, len) <= 20);
	start = 0;
	num = 1;
	nb_ok = 0;
	first_err = 0;
	err[0] = '\0';
	while (start < len)
	{
		end = start;
		while (end < len && content[end] != '\n')
			end++;
		if (end < len)
			end++;
		line = get_next_line(fd);
		if (line && strlen(line) == end - start
			&& memcmp(line, content + start, end - start) == 0)
			nb_ok++;
		else if (!first_err)
		{
			first_err = num;
			snprintf(err, sizeof(err), "ligne %d : obtenu %s, attendu %s", num,
				show(line, line ? strlen(line) : 0, b1),
				show(content + start, end - start, b2));
		}
		if (verbose)
			CHECK(line && strlen(line) == end - start
				&& memcmp(line, content + start, end - start) == 0,
				"ligne %d : obtenu %s, attendu %s", num,
				show(line, line ? strlen(line) : 0, b1),
				show(content + start, end - start, b2));
		free(line);
		if (!line && !verbose)
			break ;
		start = end;
		num++;
	}
	if (!verbose)
		CHECK(first_err == 0, "%d lignes lues correctement sur %d%s%s",
			nb_ok, count_lines(content, len), first_err ? " ; premiere erreur, " : "",
			err);
	line = get_next_line(fd);
	CHECK(line == NULL,
		"apres la derniere ligne, get_next_line doit renvoyer NULL (obtenu %s)",
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
	CHECK(fd != -1, "creation du fichier de test");
	if (fd == -1)
		return ;
	check_fd(fd, g_content, g_len);
	close(fd);
}

/* la longueur est calculee par sizeof : pas d'erreur de comptage */
#define FILE_TEST(name, lit) file_test(name, lit, sizeof(lit) - 1)

static void	file_test(const char *name, const char *content, size_t len)
{
	g_content = content;
	g_len = len;
	run_test(name, t_file);
}

/* stdin : le contenu arrive d'un coup dans un pipe */
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
		if (write(p[1], content, strlen(content)) < 0)
			exit(1);
		close(p[1]);
		exit(0);
	}
	close(p[1]);
	dup2(p[0], 0);
	close(p[0]);
	check_fd(0, content, strlen(content));
	waitpid(pid, NULL, 0);
}

/*
** stdin "lent" : le contenu arrive par petits morceaux, avec des pauses,
** comme quand on tape au clavier. read renvoie alors moins d'octets que
** BUFFER_SIZE, et parfois au milieu d'une ligne.
*/
static void	t_stdin_slow(void)
{
	const char		*content = "on tape\nune ligne\npuis une autre qui est plus longue\nfin";
	struct timespec	pause;
	int				p[2];
	pid_t			pid;
	size_t			i;

	if (pipe(p) == -1)
		return ;
	pid = fork();
	if (pid == 0)
	{
		close(p[0]);
		pause.tv_sec = 0;
		pause.tv_nsec = 3000000;
		i = 0;
		while (content[i])
		{
			if (write(p[1], content + i, 3 < strlen(content + i) ? 3 : strlen(content + i)) < 0)
				exit(1);
			i += 3 < strlen(content + i) ? 3 : strlen(content + i);
			nanosleep(&pause, NULL);
		}
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
	int		fd;

	line = get_next_line(-1);
	CHECK(line == NULL, "get_next_line(-1) doit renvoyer NULL");
	free(line);
	line = get_next_line(4242);
	CHECK(line == NULL, "get_next_line(4242), fd jamais ouvert : doit renvoyer NULL");
	free(line);
	fd = open(g_dir, O_RDONLY);
	line = get_next_line(fd);
	CHECK(line == NULL,
		"get_next_line sur le fd d'un dossier (read renvoie -1) : doit renvoyer NULL");
	free(line);
	close(fd);
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

/*
** Lignes de longueur BUFFER_SIZE - 1, BUFFER_SIZE et BUFFER_SIZE + 1 :
** c'est la ou les erreurs de decoupage apparaissent.
*/
static void	t_around_buffer_size(void)
{
	char	*content;
	size_t	n;
	size_t	len;
	int		k;
	int		fd;

	if (BUFFER_SIZE > 100000)
	{
		CHECK(1, "BUFFER_SIZE trop grand pour ce test, ignore");
		return ;
	}
	content = malloc(3 * (BUFFER_SIZE + 2) * 2 + 10);
	if (!content)
		return ;
	len = 0;
	k = -1;
	while (k <= 1)
	{
		n = 0;
		while ((long)n < (long)BUFFER_SIZE + k)
			content[len + n++] = 'a' + (k + 1);
		len += n;
		content[len++] = '\n';
		k++;
	}
	k = -1;
	while (k <= 1 && (long)BUFFER_SIZE + k > 0)
	{
		n = 0;
		while ((long)n < (long)BUFFER_SIZE + k - 1)
			content[len + n++] = 'x';
		len += n;
		content[len++] = '\n';
		k++;
	}
	fd = open(make_file(content, len), O_RDONLY);
	check_fd(fd, content, len);
	close(fd);
	free(content);
}

/* fichier qui fait exactement BUFFER_SIZE, puis 2 * BUFFER_SIZE octets */
static void	t_exact_buffer_size(void)
{
	char	*content;
	size_t	len;
	size_t	i;
	int		fd;
	int		mult;

	if (BUFFER_SIZE > 100000)
	{
		CHECK(1, "BUFFER_SIZE trop grand pour ce test, ignore");
		return ;
	}
	mult = 1;
	while (mult <= 2)
	{
		len = BUFFER_SIZE * mult;
		content = malloc(len + 1);
		if (!content)
			return ;
		i = 0;
		while (i < len)
		{
			content[i] = (i % 7 == 6) ? '\n' : 'a' + i % 26;
			i++;
		}
		fd = open(make_file(content, len), O_RDONLY);
		check_fd(fd, content, len);
		close(fd);
		free(content);
		mult++;
	}
}

int	main(void)
{
	static char	long_line[20001];
	static char	long_then_short[5020];
	static char	many[20000];
	static char	big_file[200000];
	char		title[64];
	size_t		n;
	size_t		nb;
	int			i;
	int			j;
	int			status;

	if (!mkdtemp(g_dir))
		return (1);
	sprintf(title, "get_next_line - BUFFER_SIZE = %d", (int)BUFFER_SIZE);
	print_title(title);
	if (BUFFER_SIZE <= 0)
	{
		g_content = "une ligne\nune autre\n";
		g_len = 0;
		run_test("BUFFER_SIZE <= 0 : fd invalides", t_invalid_fd);
		FILE_TEST("BUFFER_SIZE <= 0 : fichier vide", "");
	}
	else
	{
		memset(long_line, 'x', 20000);
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
		nb = 0;
		i = 0;
		while (i < 2000)
		{
			nb += sprintf(big_file + nb, "ligne numero %d du gros fichier\n", i);
			i++;
		}
		FILE_TEST("fichier vide", "");
		FILE_TEST("une ligne sans '\\n' (\"a\")", "a");
		FILE_TEST("une ligne avec '\\n' (\"a\\n\")", "a\n");
		FILE_TEST("\"abcd\\nefgh\\nij\" (pas de '\\n' final)", "abcd\nefgh\nij");
		FILE_TEST("\"abcd\\nefgh\\nij\\n\"", "abcd\nefgh\nij\n");
		FILE_TEST("un seul '\\n'", "\n");
		FILE_TEST("que des '\\n'", "\n\n\n\n");
		FILE_TEST("lignes vides au milieu", "\nabc\n\n\ndef\n\nfin");
		FILE_TEST("lignes avec '\\r' (fin de ligne Windows)", "un\r\ndeux\r\n\r\n");
		FILE_TEST("espaces et tabulations", "  \t  \n\t\n   fin  ");
		file_test("une ligne de 20000 caracteres", long_line, 20000);
		file_test("ligne longue puis ligne courte", long_then_short, 5005);
		file_test("300 lignes de longueurs variees", many, n);
		file_test("2000 lignes (gros fichier)", big_file, nb);
		run_test("lignes de BUFFER_SIZE - 1, BUFFER_SIZE et BUFFER_SIZE + 1",
			t_around_buffer_size);
		run_test("fichier de exactement BUFFER_SIZE et 2 * BUFFER_SIZE octets",
			t_exact_buffer_size);
		run_test("lecture sur l'entree standard (fd 0)", t_stdin);
		run_test("entree standard lente (texte qui arrive par morceaux)", t_stdin_slow);
		run_test("fd invalides (-1, 4242, un dossier)", t_invalid_fd);
		run_test("deux fichiers l'un apres l'autre", t_two_files);
	}
	status = print_summary();
	sprintf(g_cmd, "rm -rf %s", g_dir);
	if (system(g_cmd) != 0)
		return (status);
	return (status);
}
