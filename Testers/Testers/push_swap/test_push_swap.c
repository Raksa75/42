#define TEST_TIMEOUT 300
#include "../common/tester.h"
#include <poll.h>
#include <limits.h>
#include <math.h>
#include <errno.h>

#define RUN_TIMEOUT 10

typedef struct s_run
{
	char	*out;
	size_t	out_len;
	char	*err;
	size_t	err_len;
	int		status;
}	t_run;

typedef struct s_res
{
	int		ok;
	long	nops;
	long	counts[11];
	char	why[512];
}	t_res;

static const char	*g_ops[] = {"sa", "sb", "ss", "pa", "pb", "ra", "rb", "rr",
	"rra", "rrb", "rrr"};
static const char	*g_flags[] = {"", "--simple", "--medium", "--complex",
	"--adaptive"};
static const char	*g_ps;
static int			g_saved = 0;

static void	append(char **buf, size_t *len, size_t *cap, char *src, size_t n)
{
	if (*len + n + 1 > *cap)
	{
		*cap = (*len + n + 1) * 2;
		*buf = realloc(*buf, *cap);
	}
	memcpy(*buf + *len, src, n);
	*len += n;
	(*buf)[*len] = '\0';
}

static void	run_free(t_run *r)
{
	free(r->out);
	free(r->err);
	r->out = NULL;
	r->err = NULL;
}

static int	run_ps(char **args, t_run *r)
{
	int				po[2];
	int				pe[2];
	pid_t			pid;
	struct pollfd	fds[2];
	char			buf[65536];
	ssize_t			n;
	size_t			cap[2];
	int				open_fds;
	int				i;
	char			**argv;
	int				argc;

	argc = 0;
	while (args[argc])
		argc++;
	argv = malloc(sizeof(char *) * (argc + 2));
	argv[0] = (char *)g_ps;
	i = -1;
	while (++i < argc)
		argv[i + 1] = args[i];
	argv[argc + 1] = NULL;
	memset(r, 0, sizeof(*r));
	cap[0] = 0;
	cap[1] = 0;
	append(&r->out, &r->out_len, &cap[0], "", 0);
	append(&r->err, &r->err_len, &cap[1], "", 0);
	if (pipe(po) == -1 || pipe(pe) == -1)
		return (free(argv), -1);
	pid = fork();
	if (pid == 0)
	{
		dup2(po[1], 1);
		dup2(pe[1], 2);
		close(po[0]);
		close(po[1]);
		close(pe[0]);
		close(pe[1]);
		alarm(RUN_TIMEOUT);
		execv(g_ps, argv);
		_exit(127);
	}
	free(argv);
	close(po[1]);
	close(pe[1]);
	fds[0].fd = po[0];
	fds[1].fd = pe[0];
	fds[0].events = POLLIN;
	fds[1].events = POLLIN;
	open_fds = 2;
	while (open_fds > 0)
	{
		if (poll(fds, 2, -1) < 0 && errno != EINTR)
			break ;
		i = -1;
		while (++i < 2)
		{
			if (fds[i].fd < 0 || !(fds[i].revents & (POLLIN | POLLHUP)))
				continue ;
			n = read(fds[i].fd, buf, sizeof(buf));
			if (n <= 0)
			{
				close(fds[i].fd);
				fds[i].fd = -1;
				open_fds--;
			}
			else if (i == 0)
				append(&r->out, &r->out_len, &cap[0], buf, n);
			else
				append(&r->err, &r->err_len, &cap[1], buf, n);
		}
	}
	waitpid(pid, &r->status, 0);
	return (0);
}

static const char	*crash_text(t_run *r)
{
	static char	s[128];

	if (WIFSIGNALED(r->status) && WTERMSIG(r->status) == SIGALRM)
		return ("TIMEOUT (plus de 10 s : boucle infinie ?)");
	if (WIFSIGNALED(r->status))
	{
		snprintf(s, sizeof(s), "CRASH (%s)", strsignal(WTERMSIG(r->status)));
		return (s);
	}
	if (WIFEXITED(r->status) && WEXITSTATUS(r->status) == 127)
		return ("le programme n'a pas pu etre lance");
	return (NULL);
}

static char	**make_args(const char *flag, const int *v, int n)
{
	char	**a;
	int		i;
	int		k;

	a = malloc(sizeof(char *) * (n + 3));
	k = 0;
	if (flag && *flag)
		a[k++] = strdup(flag);
	i = -1;
	while (++i < n)
	{
		a[k] = malloc(16);
		sprintf(a[k++], "%d", v[i]);
	}
	a[k] = NULL;
	return (a);
}

static void	free_args(char **a)
{
	int	i;

	i = 0;
	while (a[i])
		free(a[i++]);
	free(a);
}

static char	*desc_input(const char *flag, const int *v, int n, char *out)
{
	int	i;
	int	j;

	j = sprintf(out, "./push_swap%s%s", (flag && *flag) ? " " : "", flag ? flag : "");
	i = -1;
	while (++i < n && i < 12)
		j += sprintf(out + j, " %d", v[i]);
	if (n > 12)
		sprintf(out + j, " ... (%d nombres)", n);
	return (out);
}

static const char	*save_input(const char *flag, const int *v, int n)
{
	static char	path[512];
	const char	*dir;
	FILE		*f;
	int			i;

	dir = getenv("FAIL_DIR");
	if (!dir || !*dir || n <= 12)
		return (NULL);
	snprintf(path, sizeof(path), "%s/push_swap_echec_%d_%d.txt", dir, (int)getpid(),
		++g_saved);
	f = fopen(path, "w");
	if (!f)
		return (NULL);
	if (flag && *flag)
		fprintf(f, "%s ", flag);
	i = -1;
	while (++i < n)
		fprintf(f, "%d%s", v[i], i + 1 < n ? " " : "\n");
	fclose(f);
	return (path);
}

static int	op_index(const char *s, size_t len)
{
	int	i;

	i = -1;
	while (++i < 11)
		if (strlen(g_ops[i]) == len && strncmp(g_ops[i], s, len) == 0)
			return (i);
	return (-1);
}

static void	apply(int op, int *a, int *na, int *b, int *nb)
{
	int	t;

	if ((op == 0 || op == 2) && *na > 1)
	{
		t = a[0];
		a[0] = a[1];
		a[1] = t;
	}
	if ((op == 1 || op == 2) && *nb > 1)
	{
		t = b[0];
		b[0] = b[1];
		b[1] = t;
	}
	if (op == 3 && *nb > 0)
	{
		memmove(a + 1, a, sizeof(int) * (*na)++);
		a[0] = b[0];
		memmove(b, b + 1, sizeof(int) * --(*nb));
	}
	if (op == 4 && *na > 0)
	{
		memmove(b + 1, b, sizeof(int) * (*nb)++);
		b[0] = a[0];
		memmove(a, a + 1, sizeof(int) * --(*na));
	}
	if ((op == 5 || op == 7) && *na > 1)
	{
		t = a[0];
		memmove(a, a + 1, sizeof(int) * (*na - 1));
		a[*na - 1] = t;
	}
	if ((op == 6 || op == 7) && *nb > 1)
	{
		t = b[0];
		memmove(b, b + 1, sizeof(int) * (*nb - 1));
		b[*nb - 1] = t;
	}
	if ((op == 8 || op == 10) && *na > 1)
	{
		t = a[*na - 1];
		memmove(a + 1, a, sizeof(int) * (*na - 1));
		a[0] = t;
	}
	if ((op == 9 || op == 10) && *nb > 1)
	{
		t = b[*nb - 1];
		memmove(b + 1, b, sizeof(int) * (*nb - 1));
		b[0] = t;
	}
}

static void	check_ops(const int *v, int n, t_run *r, t_res *res)
{
	int		*a;
	int		*b;
	int		na;
	int		nb;
	size_t	i;
	size_t	start;
	int		op;
	int		k;
	char	tmp[64];

	memset(res, 0, sizeof(*res));
	a = malloc(sizeof(int) * (n + 1));
	b = malloc(sizeof(int) * (n + 1));
	memcpy(a, v, sizeof(int) * n);
	na = n;
	nb = 0;
	res->ok = 1;
	i = 0;
	while (i < r->out_len && res->ok)
	{
		start = i;
		while (i < r->out_len && r->out[i] != '\n')
			i++;
		op = op_index(r->out + start, i - start);
		if (op < 0)
		{
			snprintf(tmp, sizeof(tmp), "%.*s", (int)(i - start > 20 ? 20 : i - start),
				r->out + start);
			snprintf(res->why, sizeof(res->why),
				"ligne %ld : \"%s\" n'est pas une operation (seulement sa sb ss pa pb ra rb rr rra rrb rrr, une par ligne)",
				res->nops + 1, tmp);
			res->ok = 0;
			break ;
		}
		apply(op, a, &na, b, &nb);
		res->counts[op]++;
		res->nops++;
		i++;
	}
	if (res->ok && nb != 0)
	{
		snprintf(res->why, sizeof(res->why), "la pile b n'est pas vide a la fin (%d nombres)", nb);
		res->ok = 0;
	}
	k = 0;
	while (res->ok && ++k < na)
	{
		if (a[k - 1] > a[k])
		{
			snprintf(res->why, sizeof(res->why),
				"la pile a n'est pas triee a la fin (%d avant %d, position %d)", a[k - 1], a[k], k);
			res->ok = 0;
		}
	}
	free(a);
	free(b);
}

static double	disorder(const int *v, int n)
{
	long	mistakes;
	long	pairs;
	int		i;
	int		j;

	mistakes = 0;
	pairs = 0;
	i = -1;
	while (++i < n)
	{
		j = i;
		while (++j < n)
		{
			pairs++;
			if (v[i] > v[j])
				mistakes++;
		}
	}
	if (pairs == 0)
		return (0);
	return ((double)mistakes / pairs);
}

static int	sort_case(const char *flag, const int *v, int n, long *nops)
{
	char		**args;
	t_run		r;
	t_res		res;
	char		d[512];
	const char	*crash;
	const char	*saved;
	int			ok;

	args = make_args(flag, v, n);
	run_ps(args, &r);
	free_args(args);
	desc_input(flag, v, n, d);
	crash = crash_text(&r);
	if (crash)
	{
		saved = save_input(flag, v, n);
		CHECK(0, "%s : %s%s%s", d, crash, saved ? "\n          entree complete : " : "",
			saved ? saved : "");
		run_free(&r);
		return (0);
	}
	check_ops(v, n, &r, &res);
	ok = res.ok && r.err_len == 0;
	if (!ok)
		saved = save_input(flag, v, n);
	else
		saved = NULL;
	if (!res.ok)
		CHECK(0, "%s : KO, %s%s%s", d, res.why, saved ? "\n          entree complete : " : "",
			saved ? saved : "");
	else if (r.err_len)
		CHECK(0, "%s : trie, mais ecrit sur la sortie d'erreur sans --bench : \"%.40s\"",
			d, r.err);
	else
		CHECK(1, "%s : OK en %ld operations", d, res.nops);
	if (nops)
		*nops = res.nops;
	run_free(&r);
	return (ok);
}

static unsigned int	g_seed = 42;

static int	rnd(int max)
{
	g_seed = g_seed * 1103515245u + 12345u;
	return ((int)((g_seed >> 8) % (unsigned int)max));
}

static void	shuffle(int *v, int n)
{
	int	i;
	int	j;
	int	t;

	i = n;
	while (--i > 0)
	{
		j = rnd(i + 1);
		t = v[i];
		v[i] = v[j];
		v[j] = t;
	}
}

static void	random_input(int *v, int n)
{
	int	i;

	i = -1;
	while (++i < n)
		v[i] = i * 37 - n * 18 + rnd(30);
	shuffle(v, n);
}

static void	towards_disorder(int *v, int n, double target)
{
	int	i;
	int	j;
	int	t;
	int	guard;

	i = -1;
	while (++i < n)
		v[i] = i * 10 - n * 5;
	guard = 0;
	while (disorder(v, n) < target && guard++ < 100000)
	{
		i = rnd(n);
		j = i + 1 + rnd(n / 4 + 1);
		if (j >= n)
			continue ;
		t = v[i];
		v[i] = v[j];
		v[j] = t;
	}
}

static void	t_noargs(void)
{
	char	*none[] = {NULL};
	t_run	r;

	run_ps(none, &r);
	CHECK(!crash_text(&r), "./push_swap sans argument : %s", crash_text(&r) ? crash_text(&r) : "ne plante pas");
	CHECK(r.out_len == 0 && r.err_len == 0,
		"./push_swap sans argument : ne doit rien afficher (obtenu %zu caracteres sur la sortie, %zu sur la sortie d'erreur)",
		r.out_len, r.err_len);
	run_free(&r);
}

static void	split_args(const char *line, char **out)
{
	static char	buf[1024];
	char		*p;
	int			k;

	strcpy(buf, line);
	k = 0;
	p = strtok(buf, "|");
	while (p)
	{
		if (strcmp(p, "@") == 0)
			*p = '\0';
		out[k++] = p;
		p = strtok(NULL, "|");
	}
	out[k] = NULL;
}

static void	t_errors(void)
{
	const char	*cases[] = {"1|a|2", "one", "1|2|1", "2147483648", "-2147483649",
		"99999999999999999999", "1|2|99999999999", "+", "-", "1-2", "2a", "1|2|2",
		"4|5|+-3", "5|--3", "1|2|3|4|5|1", "--simple|3|2|3", "--adaptive|0|one|2|3",
		"--complex|1|x", "--medium|42|42", "1|2|3.5", "--inconnu|1|2", NULL};
	char		*args[32];
	char		d[256];
	t_run		r;
	int			i;
	int			j;

	i = -1;
	while (cases[++i])
	{
		split_args(cases[i], args);
		j = sprintf(d, "./push_swap");
		for (int k = 0; args[k]; k++)
			j += sprintf(d + j, " %s", args[k]);
		run_ps(args, &r);
		if (crash_text(&r))
			CHECK(0, "%s : %s (attendu \"Error\")", d, crash_text(&r));
		else
			CHECK(r.out_len == 0 && r.err_len == 6 && strcmp(r.err, "Error\n") == 0,
				"%s : sortie d'erreur %s (attendu \"Error\\n\"), sortie standard %s (attendu rien)",
				d, r.err_len ? r.err : "vide", r.out_len ? "non vide" : "vide");
		run_free(&r);
	}
}

static void	edge(const char *line, const char *expect, int want_error)
{
	char	*args[32];
	char	d[256];
	t_run	r;
	int		j;
	int		is_err;

	split_args(line, args);
	j = sprintf(d, "./push_swap");
	for (int k = 0; args[k]; k++)
		j += sprintf(d + j, " \"%s\"", args[k]);
	run_ps(args, &r);
	if (crash_text(&r))
	{
		CHECK(0, "%s : %s", d, crash_text(&r));
		run_free(&r);
		return ;
	}
	CHECK(1, "%s : ne plante pas", d);
	is_err = (strcmp(r.err, "Error\n") == 0);
	if (want_error == 1 && !is_err)
		NOTE("%s : %s, mais ton programme ne renvoie pas Error", d, expect);
	else if (want_error == 0 && is_err)
		NOTE("%s : %s, mais ton programme renvoie Error", d, expect);
	else
		log_printf("      %s : %s (comportement de ton programme : %s)\n", d, expect,
			is_err ? "Error" : (r.out_len ? "affiche des operations" : "n'affiche rien"));
	run_free(&r);
}

static void	t_edge_formats(void)
{
	edge("@", "un argument vide n'est pas un entier : Error attendu", 1);
	edge("1|@|2", "un argument vide au milieu : Error attendu", 1);
	edge("1| |2", "un argument fait d'un espace : Error attendu", 1);
	edge("0|-0", "0 et -0 sont le meme nombre (doublon) : Error attendu", 1);
	edge("+5|3", "+5 est un entier valide", 0);
	edge("007|3", "007 est l'entier 7", 0);
	edge("-2147483648|2147483647", "INT_MIN et INT_MAX sont valides", 0);
	edge("3 2 1", "tous les nombres dans un seul argument : le sujet ne le demande pas, au choix", 2);
	edge("--simple", "une option sans nombres : ne doit pas planter", 2);
	edge("--bench", "--bench sans nombres : ne doit pas planter", 2);
	edge("--simple|--complex|3|2|1", "deux options : au choix, mais ne doit pas planter", 2);
	edge("3|2|1|--simple", "option apres les nombres : au choix, mais ne doit pas planter", 2);
}

static void	t_sorted(void)
{
	int		v[100];
	int		sizes[] = {1, 2, 3, 5, 100};
	char	**args;
	char	d[512];
	char	o[1024];
	t_run	r;
	int		f;
	int		s;
	int		i;

	f = -1;
	while (++f < 5)
	{
		s = -1;
		while (++s < 5)
		{
			i = -1;
			while (++i < sizes[s])
				v[i] = i * 3 - 7;
			args = make_args(g_flags[f], v, sizes[s]);
			run_ps(args, &r);
			free_args(args);
			desc_input(g_flags[f], v, sizes[s], d);
			if (crash_text(&r))
				CHECK(0, "%s : %s", d, crash_text(&r));
			else
				CHECK(r.out_len == 0, "%s (deja trie) : ne doit rien afficher, obtenu %s",
					d, show(r.out, r.out_len, o));
			run_free(&r);
		}
	}
}

static int	next_perm(int *v, int n)
{
	int	i;
	int	j;
	int	t;

	i = n - 2;
	while (i >= 0 && v[i] >= v[i + 1])
		i--;
	if (i < 0)
		return (0);
	j = n - 1;
	while (v[j] <= v[i])
		j--;
	t = v[i];
	v[i] = v[j];
	v[j] = t;
	j = n - 1;
	i++;
	while (i < j)
	{
		t = v[i];
		v[i++] = v[j];
		v[j--] = t;
	}
	return (1);
}

static void	all_perms(int n, int limit_note)
{
	int		v[8];
	long	nops;
	long	worst;
	int		f;
	int		i;
	int		count;
	char	d[256];
	char	wd[256];

	f = -1;
	while (++f < 5)
	{
		i = -1;
		while (++i < n)
			v[i] = (i + 1) * 4 - 9;
		worst = -1;
		count = 0;
		do
		{
			count++;
			nops = 0;
			if (sort_case(g_flags[f], v, n, &nops) && nops > worst)
			{
				worst = nops;
				desc_input(g_flags[f], v, n, wd);
			}
		} while (next_perm(v, n));
		if (worst > limit_note && limit_note > 0)
			NOTE("%s : %ld operations (les fiches de correction demandent souvent %d au maximum pour %d nombres)",
				wd, worst, limit_note, n);
		sprintf(d, "%s", g_flags[f][0] ? g_flags[f] : "sans option");
		log_printf("      %s : %d permutations, %ld operations au pire\n", d, count, worst);
	}
}

static void	t_small3(void)
{
	all_perms(2, 1);
	all_perms(3, 3);
}

static void	t_small5(void)
{
	all_perms(4, 0);
	all_perms(5, 12);
}

static void	strategy_cases(const char *flag)
{
	int		v[500];
	int		sizes[] = {6, 7, 10, 25, 50, 100, 250, 500};
	int		s;
	int		k;
	int		i;

	s = -1;
	while (++s < 8)
	{
		k = -1;
		while (++k < (sizes[s] >= 250 ? 2 : 4))
		{
			random_input(v, sizes[s]);
			sort_case(flag, v, sizes[s], NULL);
		}
	}
	i = -1;
	while (++i < 100)
		v[i] = 500 - i * 5;
	sort_case(flag, v, 100, NULL);
	towards_disorder(v, 100, 0.05);
	sort_case(flag, v, 100, NULL);
	towards_disorder(v, 100, 0.35);
	sort_case(flag, v, 100, NULL);
	i = -1;
	while (++i < 100)
		v[i] = i + 1;
	v[0] = 100;
	v[99] = 1;
	sort_case(flag, v, 100, NULL);
}

static void	t_default(void)
{
	strategy_cases("");
}

static void	t_simple(void)
{
	strategy_cases("--simple");
}

static void	t_medium(void)
{
	strategy_cases("--medium");
}

static void	t_complex(void)
{
	strategy_cases("--complex");
}

static void	t_adaptive(void)
{
	strategy_cases("--adaptive");
}

static void	t_extremes(void)
{
	int	a[] = {INT_MAX, INT_MIN, 0};
	int	b[] = {-1, -2, -3, -4, -5, -6};
	int	c[] = {INT_MIN, INT_MAX, -1, 1, 0, INT_MIN + 1, INT_MAX - 1};
	int	d[] = {2147483647, -2147483648, 2147483646, -2147483647, 5, -5, 1000000, -1000000};
	int	f;

	f = -1;
	while (++f < 5)
	{
		sort_case(g_flags[f], a, 3, NULL);
		sort_case(g_flags[f], b, 6, NULL);
		sort_case(g_flags[f], c, 7, NULL);
		sort_case(g_flags[f], d, 8, NULL);
	}
}

static long	bench_count(const char *err, const char *op)
{
	const char	*p;
	size_t		len;

	len = strlen(op);
	p = err;
	while ((p = strstr(p, op)))
	{
		if ((p == err || p[-1] == ' ' || p[-1] == ']' || p[-1] == '\t')
			&& p[len] == ':')
			return (atol(p + len + 1));
		p += len;
	}
	return (-1);
}

static int	class_match(const char *line, const char *flag)
{
	if (strcmp(flag, "--simple") == 0)
		return (strstr(line, "n2") || strstr(line, "n^2") || strstr(line, "n²")
			|| strstr(line, "n * n") || strstr(line, "n*n"));
	if (strcmp(flag, "--medium") == 0)
		return (strstr(line, "√") || strcasestr(line, "sqrt") || strstr(line, "1.5")
			|| strstr(line, "^(3/2)") || strstr(line, "^1.5"));
	if (strcmp(flag, "--complex") == 0)
		return (strcasestr(line, "log") != NULL);
	return (strcasestr(line, "adapt") != NULL);
}

static void	get_line_after(const char *err, const char *key, char *out, size_t size)
{
	const char	*p;
	size_t		i;

	out[0] = '\0';
	p = strstr(err, key);
	if (!p)
		return ;
	p += strlen(key);
	while (*p == ' ')
		p++;
	i = 0;
	while (p[i] && p[i] != '\n' && i + 1 < size)
	{
		out[i] = p[i];
		i++;
	}
	out[i] = '\0';
}

static void	bench_case(const char *flag, int *v, int n)
{
	char	*args[512];
	char	d[512];
	char	strat[256];
	char	dis[64];
	char	want[32];
	t_run	r;
	t_res	res;
	int		k;
	int		i;

	k = 0;
	args[k++] = "--bench";
	if (*flag)
		args[k++] = (char *)flag;
	i = -1;
	while (++i < n)
	{
		args[k] = malloc(16);
		sprintf(args[k++], "%d", v[i]);
	}
	args[k] = NULL;
	run_ps(args, &r);
	i = (*flag) ? 2 : 1;
	while (args[i])
		free(args[i++]);
	sprintf(d, "--bench%s%s (%d nombres)", *flag ? " " : "", flag, n);
	if (crash_text(&r))
	{
		CHECK(0, "%s : %s", d, crash_text(&r));
		run_free(&r);
		return ;
	}
	check_ops(v, n, &r, &res);
	CHECK(res.ok, "%s : la sortie standard trie toujours la pile%s%s", d, res.ok ? "" : " : ",
		res.why);
	CHECK(r.err_len > 0, "%s : les statistiques doivent etre sur la sortie d'erreur", d);
	get_line_after(r.err, "disorder:", dis, sizeof(dis));
	sprintf(want, "%.2f%%", disorder(v, n) * 100);
	CHECK(dis[0] && fabs(atof(dis) - disorder(v, n) * 100) < 0.006,
		"%s : disorder affiche \"%s\", attendu %s", d, dis[0] ? dis : "(absent)", want);
	get_line_after(r.err, "strategy:", strat, sizeof(strat));
	CHECK(strat[0] && class_match(strat, *flag ? flag : "--adaptive"),
		"%s : strategy affiche \"%s\", attendu le nom de la strategie et sa complexite%s",
		d, strat[0] ? strat : "(absent)",
		*flag && strcmp(flag, "--adaptive") ? " (ex : O(n²), O(n√n), O(n log n))" : " (Adaptive / ...)");
	get_line_after(r.err, "total_ops:", dis, sizeof(dis));
	CHECK(dis[0] && atol(dis) == res.nops, "%s : total_ops affiche %s, il y a %ld operations sur la sortie",
		d, dis[0] ? dis : "(absent)", res.nops);
	i = -1;
	while (++i < 11)
		CHECK(bench_count(r.err, g_ops[i]) == res.counts[i],
			"%s : %s affiche %ld, il y en a %ld sur la sortie", d, g_ops[i],
			bench_count(r.err, g_ops[i]), res.counts[i]);
	run_free(&r);
}

static void	t_bench(void)
{
	int	v[100];
	int	five[] = {4, 67, 3, 87, 23};
	int	f;

	bench_case("--adaptive", five, 5);
	f = -1;
	while (++f < 5)
	{
		random_input(v, 100);
		bench_case(g_flags[f], v, 100);
	}
	towards_disorder(v, 60, 0.1);
	bench_case("", v, 60);
}

static void	t_adaptive_regimes(void)
{
	double	targets[] = {0.08, 0.15, 0.3, 0.42, 0.6, 0.9};
	const char	*cls[] = {"--simple", "--simple", "--medium", "--medium", "--complex",
		"--complex"};
	const char	*names[] = {"O(n²)", "O(n²)", "O(n√n)", "O(n√n)", "O(n log n)", "O(n log n)"};
	int		v[150];
	char	*args[160];
	char	strat[256];
	t_run	r;
	t_res	res;
	double	dz;
	int		t;
	int		i;
	int		k;

	t = -1;
	while (++t < 6)
	{
		if (targets[t] >= 0.5)
		{
			towards_disorder(v, 150, 1.0 - targets[t] - 0.02);
			i = -1;
			while (++i < 75)
			{
				k = v[i];
				v[i] = v[149 - i];
				v[149 - i] = k;
			}
		}
		else
			towards_disorder(v, 150, targets[t]);
		dz = disorder(v, 150);
		k = 0;
		args[k++] = "--bench";
		i = -1;
		while (++i < 150)
		{
			args[k] = malloc(16);
			sprintf(args[k++], "%d", v[i]);
		}
		args[k] = NULL;
		run_ps(args, &r);
		i = 1;
		while (args[i])
			free(args[i++]);
		if (crash_text(&r))
		{
			CHECK(0, "desordre %.2f%% : %s", dz * 100, crash_text(&r));
			run_free(&r);
			continue ;
		}
		check_ops(v, 150, &r, &res);
		CHECK(res.ok, "desordre %.2f%% : %s", dz * 100, res.ok ? "trie" : res.why);
		get_line_after(r.err, "strategy:", strat, sizeof(strat));
		CHECK(class_match(strat, cls[t]),
			"desordre %.2f%% (%s 0.2 / 0.5) : strategy \"%s\", attendu la methode en %s",
			dz * 100, dz < 0.2 ? "<" : (dz < 0.5 ? "entre" : ">="), strat[0] ? strat : "(absent)",
			names[t]);
		INFO("desordre %5.2f%% : %5ld operations, %s", dz * 100, res.nops, strat);
		run_free(&r);
	}
}

static void	perf(int n, int runs, int lim[3])
{
	int		v[500];
	long	nops;
	long	total;
	long	worst;
	long	best;
	int		k;
	int		ok;
	const char	*lvl;

	total = 0;
	worst = 0;
	best = LONG_MAX;
	ok = 0;
	k = -1;
	while (++k < runs)
	{
		random_input(v, n);
		nops = 0;
		if (sort_case("", v, n, &nops))
		{
			ok++;
			total += nops;
			if (nops > worst)
				worst = nops;
			if (nops < best)
				best = nops;
		}
	}
	if (ok == 0)
		return ;
	if (worst < lim[0])
		lvl = GREEN "excellent" RESET;
	else if (worst < lim[1])
		lvl = GREEN "bien" RESET;
	else if (worst < lim[2])
		lvl = YELLOW "suffisant" RESET;
	else
		lvl = RED "insuffisant" RESET;
	CHECK(worst < lim[2], "%d nombres : pire cas %ld operations, il faut moins de %d pour valider",
		n, worst, lim[2]);
	INFO("%d listes de %d nombres : moyenne %ld, min %ld, max %ld -> %s  "
		DIM "(excellent < %d, bien < %d, valide < %d)" RESET,
		ok, n, total / ok, best, worst, lvl, lim[0], lim[1], lim[2]);
}

static void	t_perf100(void)
{
	int	lim[3] = {700, 1500, 2000};

	perf(100, 30, lim);
}

static void	t_perf500(void)
{
	int	lim[3] = {5500, 8000, 12000};

	perf(500, 15, lim);
}

static void	t_complexity(void)
{
	const char	*flags[] = {"--simple", "--medium", "--complex"};
	const char	*names[] = {"O(n²)", "O(n√n)", "O(n log n)"};
	double		expect[] = {2.0, 1.5, 1.15};
	double		limit[] = {9, 1.8, 1.45};
	int			sizes[] = {100, 400};
	int			v[400];
	double		avg[2];
	double		e;
	long		nops;
	int			f;
	int			s;
	int			k;

	f = -1;
	while (++f < 3)
	{
		s = -1;
		while (++s < 2)
		{
			avg[s] = 0;
			k = -1;
			while (++k < 3)
			{
				random_input(v, sizes[s]);
				nops = 0;
				sort_case(flags[f], v, sizes[s], &nops);
				avg[s] += nops / 3.0;
			}
		}
		if (avg[0] < 1)
			continue ;
		e = log(avg[1] / avg[0]) / log(4.0);
		INFO("%-10s %8.0f ops pour 100, %8.0f pour 400 -> croissance en n^%.2f  " DIM
			"(%s : environ n^%.2f)" RESET, flags[f], avg[0], avg[1], e, names[f], expect[f]);
		if (e > limit[f])
			NOTE("%s grandit en n^%.2f : plus vite que la complexite %s annoncee", flags[f],
				e, names[f]);
	}
}

int	main(void)
{
	g_ps = getenv("PUSH_SWAP");
	if (!g_ps || access(g_ps, X_OK) != 0)
	{
		printf("   " RED "✘" RESET " programme push_swap introuvable : %s\n", g_ps ? g_ps : "(PUSH_SWAP vide)");
		return (1);
	}
	print_title("push_swap - arguments et erreurs");
	run_test("sans argument : rien a afficher", t_noargs);
	run_test("erreurs : \"Error\\n\" sur la sortie d'erreur", t_errors);
	run_test("cas discutables (ne doit pas planter)", t_edge_formats);
	print_title("push_swap - tri (verifie par le checker du testeur)");
	run_test("deja trie : aucune operation", t_sorted);
	run_test("2 et 3 nombres : toutes les permutations", t_small3);
	run_test("4 et 5 nombres : toutes les permutations", t_small5);
	run_test("valeurs extremes (INT_MIN, INT_MAX, negatifs)", t_extremes);
	run_test("sans option (adaptatif par defaut)", t_default);
	run_test("--simple", t_simple);
	run_test("--medium", t_medium);
	run_test("--complex", t_complex);
	run_test("--adaptive", t_adaptive);
	print_title("push_swap - --bench et strategie adaptative");
	run_test("--bench : statistiques exactes sur stderr", t_bench);
	run_test("adaptatif : la bonne methode selon le desordre", t_adaptive_regimes);
	print_title("push_swap - performances (sans option, comme en correction)");
	run_test("100 nombres aleatoires (30 listes)", t_perf100);
	run_test("500 nombres aleatoires (15 listes)", t_perf500);
	run_test("complexite mesuree de chaque strategie", t_complexity);
	return (print_summary());
}
