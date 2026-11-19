# Testers

Testers for my 42 projects. Each tester compiles the project, checks the Makefile and the
Norm, runs the tests, then checks for memory leaks with `valgrind`.

| Folder | Project tested | What is checked |
|---|---|---|
| `libft/` | [Libft](../Libft) | Makefile (rules, flags, no relink), Norm, allowed functions only, the 43 functions compared with the libc or with the expected result (about 600 checks: limits like `INT_MIN`, `'\0'`, size 0, empty strings, `SIZE_MAX`, lists of 1000 nodes), leaks |
| `printf/` | [ft_printf](../Printf) | Makefile, Norm, allowed functions only, every conversion (`cspdiuxX%`) compared with the real `printf`, output **and** return value (about 450 checks: all ASCII characters, `INT_MIN`/`INT_MAX`/`UINT_MAX`, `NULL` strings and pointers, mixed conversions), leaks |
| `gnl/` | [get_next_line](../GetNextLine) | Norm, allowed functions only, compilation with and without `BUFFER_SIZE`, `BUFFER_SIZE` from 0 to 1000: empty file, with or without a final `\n`, empty lines, very long lines, lines of `BUFFER_SIZE - 1`, `BUFFER_SIZE` and `BUFFER_SIZE + 1` characters, a 2000-line file, standard input (all at once or slowly, in small pieces), invalid fds, calls after the end of the file, leaks |

## Installation

Run once, from the root of the repository:

```bash
./Testers/install.sh
```

It installs a `raksa` command (a link in `~/.local/bin`), so the testers can be run from any
folder. Open a new terminal after the installation. If the repository is moved, run
`install.sh` again.

## Usage

Type `raksa` to open the menu. Choose with the **arrow keys** (or `j`/`k`, or the number of
the line) and confirm with **Enter**. It asks step by step:

1. what to do: test one project, test the three projects of this repo, or show the help;
2. if a project is found in the current folder, whether to test it;
3. otherwise, which project (Libft, ft_printf or get_next_line) and which folder:
   the current folder, the one of this repo, or any other path;
4. whether to look for memory leaks with `valgrind`;
5. after the test: show the log, run it again, go back to the menu, or quit.

The tests can also be started directly, without the menu:

```bash
raksa ici                # tests the project of the current folder
raksa libft [folder]     # tests a Libft (the one of this repo by default)
raksa printf [folder]    # tests a ft_printf
raksa gnl [folder]       # tests a get_next_line
raksa all                # tests the three projects of this repo
raksa log                # shows the log of the last test
raksa --no-valgrind      # faster, without the leak check
raksa -h                 # help
```

The project folder can be anywhere, for example a copy of the Libft inside another
project, or the folder of a 42 submission repository.

Without the installation, the scripts can also be run directly:

```bash
./Testers/run_all.sh
./Testers/libft/run.sh
```

## Logs

Each run writes a log file in `Testers/logs/`, named after the project and the date
(for example `libft_2026-11-19_14-30-00.log`). `Testers/logs/dernier.log` always points to
the last one. The screen only shows one line per group of tests; the log shows **every
check**, with the values used:

```
  # ft_atoi
      [OK] ft_atoi("-42") = -42, attendu -42
      [OK] ft_atoi("+42") = 42, attendu 42
      [KO] ft_atoi("+-42") = 42, attendu 0
    => KO (voir les lignes [KO] ci-dessus)
```

For a failed check, it gives the value tested, the expected result and the obtained result.
For a crash, a timeout or a leak, it says after which check it happened. The log also
contains the output of `make`, `norminette`, the compiler and `valgrind`.

To read it: `raksa log`, the "Voir le log" line of the menu, or `less Testers/logs/dernier.log`.
The `logs/` folder is in `.gitignore`, so the logs are not pushed.

## How it works

- Each test runs in its own process: a crash (`[CRASH]`) or an infinite loop (`[TIMEOUT]`)
  does not stop the other tests.
- When a test fails, the first errors are shown under it (expected value and obtained
  value); all of them are in the log.
- The leak check is skipped if `valgrind` is not installed, and the Norm check if
  `norminette` is not installed.
- If `libft.a` or `libftprintf.a` did not exist before the test, `make fclean` is run at the
  end, so no `.o` or `.a` file is left in the project folder.

## Files

```
Testers/
├── raksa.sh            # the "raksa" command: menu, project detection
├── install.sh          # installs the "raksa" command
├── assets/raksa.ans    # the picture shown in the menu (coloured terminal art)
├── run_all.sh          # runs the three testers
├── common/tester.h     # shared C code: runs each test in a child process, writes the log
├── common/common.sh    # shared shell code: Makefile, Norm, allowed functions, valgrind, log
├── logs/               # the log files (not pushed)
├── libft/              # run.sh + test_libft.c
├── printf/             # run.sh + test_printf.c
└── gnl/                # run.sh + test_gnl.c
```
