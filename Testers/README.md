# Testers

Testers for my 42 projects. Each tester compiles the project, checks the Makefile and the
Norm, runs the tests, then checks for memory leaks with `valgrind`.

| Folder | Project tested | What is checked |
|---|---|---|
| `libft/` | [Libft](../Libft) | Makefile (rules, no relink), Norm, the 43 functions compared with the libc, leaks |
| `printf/` | [ft_printf](../Printf) | Makefile, Norm, every conversion (`cspdiuxX%`) compared with the real `printf` (output **and** return value), leaks |
| `gnl/` | [get_next_line](../GetNextLine) | Norm, allowed functions only, compilation with and without `BUFFER_SIZE`, many files with `BUFFER_SIZE` from 1 to 10000000, standard input, invalid fds, leaks |

## Usage

From the root of the repository:

```bash
./Testers/run_all.sh                # all the projects
./Testers/libft/run.sh              # only one project
./Testers/gnl/run.sh --no-valgrind  # faster, without the leak check
```

- Each test runs in its own process: a crash (`[CRASH]`) or an infinite loop (`[TIMEOUT]`)
  does not stop the other tests.
- When a test fails, the details are shown under it (expected value and obtained value).
- The leak check is skipped if `valgrind` is not installed, and the Norm check if
  `norminette` is not installed.
- The testers run `make fclean` at the end, so no `.o` or `.a` file is left in the project
  folders.

## Files

```
Testers/
├── run_all.sh          # runs the three testers
├── common/tester.h     # shared code: runs each test in a child process, prints results
├── libft/              # run.sh + test_libft.c
├── printf/             # run.sh + test_printf.c
└── gnl/                # run.sh + test_gnl.c
```

These testers were written with the help of AI (Claude).
