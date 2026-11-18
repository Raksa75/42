# 42 Paris - nzimmerm

My projects from the **42 common core**, written in C following the 42 Norm.

Each project has its own folder and its own `README.md` with a full description,
compilation instructions and resources.

## Projects

| Project | Description | Status |
|---|---|---|
| [Libft](./Libft) | My own C library: libc functions, string utilities and linked lists | ✅ Done |
| [ft_printf](./Printf) | Recode of the `printf()` function, using variadic functions | ✅ Done |
| [get_next_line](./GetNextLine) | Read a file line by line from a file descriptor | ✅ Done |

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
└── GetNextLine/
    ├── README.md
    ├── get_next_line.h
    ├── get_next_line.c
    └── get_next_line_utils.c
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

Every project compiles with `cc -Wall -Wextra -Werror`. See each project's README for details.

## About 42

[42](https://42.fr) is a peer-to-peer programming school with no teachers and no
classes: students learn by doing projects, which are then evaluated by other students.
