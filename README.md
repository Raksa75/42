# 42 Paris

My projects from the **42 common core**, written in C following the 42 Norm, and a few
personal projects made to practise.

Each project has its own folder and its own `README.md` with a full description,
compilation instructions and resources.

## Projects

| Project | Description | Status |
|---|---|---|
| [Libft](./Libft) | My own C library: libc functions, string utilities and linked lists | ✅ Done |
| [ft_printf](./Printf) | Recode of the `printf()` function, using variadic functions | ✅ Done |
| [get_next_line](./GetNextLine) | Read a file line by line from a file descriptor | ✅ Done |
| push_swap | Sort a stack of numbers with a limited set of operations, with several strategies | 🚧 In progress |
| [Testers](./Testers) | My own testers for these projects, with the `raksa` menu | ✅ Done |

## Games

Small personal projects, outside the 42 curriculum, to practise what I learned.

| Game | Description | Status |
|---|---|---|
| [Pierre Feuille Ciseaux](./Games/PFC) | Rock-paper-scissors in the terminal against the computer, with a menu and a score | ✅ Done |

## Repository structure

```
.
├── README.md                    # this file
├── Libft/                       # each project in its own folder
│   ├── README.md
│   ├── Makefile
│   ├── libft.h
│   └── ft_*.c
├── Printf/
│   ├── README.md
│   ├── Makefile
│   ├── ft_printf.h
│   └── *.c
├── GetNextLine/
│   ├── README.md
│   ├── get_next_line.h
│   ├── get_next_line.c
│   └── get_next_line_utils.c
├── Games/                       # personal projects
│   └── PFC/                     # rock-paper-scissors (uses ft_printf)
└── Testers/                     # testers for the projects
    ├── README.md
    ├── raksa.sh                 # the "raksa" menu
    ├── install.sh
    └── libft/  printf/  gnl/  push_swap/
```

## Usage

Clone the repository:

```bash
git clone https://github.com/Raksa75/42.git
cd 42
```

**Libft** and **ft_printf** are libraries built with `make`:

```bash
cd Libft
make
```

Their Makefiles provide the `all`, `clean`, `fclean` and `re` rules.

**get_next_line** has no Makefile: its files are compiled directly with your own program,
choosing the buffer size with `-D BUFFER_SIZE=n`:

```bash
cd GetNextLine
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

**The games** have their own Makefile, which also builds the libraries they use:

```bash
cd Games/PFC
make
./pfc
```

Every project compiles with `cc -Wall -Wextra -Werror`. See each project's README for details.

## Testing

The [Testers](./Testers) folder checks each project: submission files and README, Makefile,
Norm, allowed functions, results compared with the original functions, and memory leaks
with `valgrind`. They also work on someone else's project, for example during a
peer-evaluation.

Install the `raksa` command once:

```bash
./Testers/install.sh
```

Then type `raksa` from any folder: a menu opens, to choose with the arrow keys and Enter
which project and which folder to test. It works on Linux and on Windows with WSL.

```bash
raksa                 # opens the menu
raksa ici             # tests the project of the current folder
raksa libft ft_split  # runs only the tests of one function
raksa eval ici        # simulates a peer-evaluation, with an estimated grade
raksa all             # tests all the projects of this repo
```

See the [Testers README](./Testers/README.md) for all the options.

## About 42

[42](https://42.fr) is a peer-to-peer programming school with no teachers and no
classes: students learn by doing projects, which are then evaluated by other students.
