# Testers

Testers for my 42 projects. Each tester compiles the project, checks the Makefile and the
Norm, runs the tests, then checks for memory leaks with `valgrind`.

| Folder | Project tested | What is checked |
|---|---|---|
| `libft/` | [Libft](../Libft) | Makefile (rules, no relink), Norm, the 43 functions compared with the libc, leaks |
| `printf/` | [ft_printf](../Printf) | Makefile, Norm, every conversion (`cspdiuxX%`) compared with the real `printf` (output **and** return value), leaks |
| `gnl/` | [get_next_line](../GetNextLine) | Norm, allowed functions only, compilation with and without `BUFFER_SIZE`, many files with `BUFFER_SIZE` from 0 to 1000, standard input, invalid fds, leaks |

## Installation

Run once, from the root of the repository:

```bash
./Testers/install.sh
```

It installs a `correcteur` command (a link in `~/.local/bin`), so the testers can be run
from any folder. Open a new terminal after the installation. If the repository is moved,
run `install.sh` again.

## Usage

```bash
correcteur                    # tests the project of the current folder
                              # (detected with libft.h, ft_printf.h or get_next_line.h)
correcteur -m                 # interactive menu
correcteur libft [folder]     # tests a Libft (the one of this repo by default)
correcteur printf [folder]    # tests a ft_printf
correcteur gnl [folder]       # tests a get_next_line
correcteur all                # tests the three projects of this repo
correcteur --no-valgrind      # faster, without the leak check
correcteur -h                 # help
```

The project folder can be anywhere, for example a copy of the Libft inside another
project, or the folder of a 42 submission repository.

Without the installation, the scripts can also be run directly:

```bash
./Testers/run_all.sh
./Testers/libft/run.sh
```

## How it works

- Each test runs in its own process: a crash (`[CRASH]`) or an infinite loop (`[TIMEOUT]`)
  does not stop the other tests.
- When a test fails, the details are shown under it (expected value and obtained value).
- The leak check is skipped if `valgrind` is not installed, and the Norm check if
  `norminette` is not installed.
- If `libft.a` or `libftprintf.a` did not exist before the test, `make fclean` is run at the
  end, so no `.o` or `.a` file is left in the project folder.

## Files

```
Testers/
├── correcteur.sh       # the "correcteur" command: menu, project detection
├── install.sh          # installs the "correcteur" command
├── run_all.sh          # runs the three testers
├── common/tester.h     # shared code: runs each test in a child process, prints results
├── libft/              # run.sh + test_libft.c
├── printf/             # run.sh + test_printf.c
└── gnl/                # run.sh + test_gnl.c
```

These testers were written with the help of AI (Claude).
