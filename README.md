# 42 Paris - nzimmerm

My projects from the **42 common core**, written in C following the 42 Norm.

Each project has its own folder and its own `README.md` with a full description,
compilation instructions and resources.

## Projects

| Project | Description | Status |
|---|---|---|
| [Libft](./Libft) | My own C library: libc functions, string utilities and linked lists | ✅ Done |
| [ft_printf](./Printf) | Recode of the printf function | 🚧 In progress |

## Repository structure

```
.
├── README.md        # this file
└── libft/           # each project in its own folder
    ├── README.md
    ├── Makefile
    ├── libft.h
    └── ft_*.c
```

## Usage

Clone the repository, then go into a project folder and build it with `make`:

```bash
git clone <repository-url>
cd <repository-name>/libft
make
```

Every project compiles with `cc -Wall -Wextra -Werror`, and its Makefile provides at
least the `all`, `clean`, `fclean` and `re` rules.

## About 42

[42](https://42.fr) is a peer-to-peer programming school with no teachers and no
classes: students learn by doing projects, which are then evaluated by other students.
