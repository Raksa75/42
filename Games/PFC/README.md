# Pierre Feuille Ciseaux

A rock-paper-scissors game in the terminal, written in C: you play against the computer,
with a menu and a score kept during the whole session.

It is a small personal project, made to practise what I learned in the 42 common core:
reading from the keyboard with `read`, using my own `ft_printf`, splitting a program into
functions, and writing a Makefile that builds another library.

## Features

- A menu: play, show the score, or quit
- The computer chooses at random
- Score: wins, losses and draws, kept until you quit
- Invalid inputs are rejected and the question is asked again
- `Ctrl+D` quits cleanly, from the menu or during a game
- No memory leak (checked with `valgrind`)

## Compilation

The game uses my [ft_printf](../../Printf), which is built automatically by the Makefile.

```bash
cd Games/PFC
make
```

| Rule | Effect |
|---|---|
| `make` | builds `ft_printf`, then the game (`pfc`) |
| `make clean` | removes the object files, also in `Printf/` |
| `make fclean` | same, and removes `pfc` and `libftprintf.a` |
| `make re` | `fclean` then `make` |

## How to play

```bash
./pfc
```

| Key | In the menu | During a game |
|---|---|---|
| `1` | Play | Rock (Pierre) |
| `2` | Show the score | Paper (Papier) |
| `3` | Quit | Scissors (Ciseaux) |
| `Ctrl+D` | Quit | Quit |

Type the number, then press Enter.

### Rules

- Rock beats scissors
- Scissors beat paper
- Paper beats rock
- Same choice: draw

## Files

```
PFC/
├── Makefile
├── pfc.h        # includes and BUFFER_SIZE
└── pfc.c        # menu, game, score
```

## What I learned

- Splitting the code between a header (`.h`, with include guards) and a source file (`.c`)
- The difference between a character and a number (`'1'` is 49), and array indexes starting at 0
- Passing an array to a function with a pointer, and why a local variable is reset at each call
- `read` returns 0 on `Ctrl+D` (end of input), and how to stop a loop cleanly
- Operator precedence: `&&` is evaluated before `||`
- A Makefile that builds another project with `$(MAKE) -C`, without relinking
- `srand(time(NULL))` must be called only once, or the computer plays the same move within the same second
