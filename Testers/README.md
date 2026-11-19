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

It installs a `raksa` command (a link in `~/.local/bin`), so the testers can be run from any
folder. Open a new terminal after the installation. If the repository is moved, run
`install.sh` again.

## Usage

Type `raksa` to open the menu. It asks step by step:

1. what to do: test one project, test the three projects of this repo, or show the help;
2. if a project is found in the current folder, whether to test it;
3. otherwise, which project (Libft, ft_printf or get_next_line) and which folder:
   the current folder, the one of this repo, or any other path;
4. whether to look for memory leaks with `valgrind`;
5. after the test: run it again, go back to the menu, or quit.

The tests can also be started directly, without the menu:

```bash
raksa ici                # tests the project of the current folder
raksa libft [folder]     # tests a Libft (the one of this repo by default)
raksa printf [folder]    # tests a ft_printf
raksa gnl [folder]       # tests a get_next_line
raksa all                # tests the three projects of this repo
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
├── raksa.sh            # the "raksa" command: menu, project detection
├── install.sh          # installs the "raksa" command
├── assets/raksa.ans    # the picture shown in the menu (coloured terminal art)
├── run_all.sh          # runs the three testers
├── common/tester.h     # shared code: runs each test in a child process, prints results
├── libft/              # run.sh + test_libft.c
├── printf/             # run.sh + test_printf.c
└── gnl/                # run.sh + test_gnl.c
```

These testers were written with the help of AI (Claude).
