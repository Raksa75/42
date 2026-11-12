*This activity has been created as part of the 42 curriculum by nzimmerm.*

# Libft

## Description

**Libft** is the first project of the 42 common core. The goal is to recode, in C, a set of
functions from the standard C library, as well as additional utility functions, and to gather
them in a static library (`libft.a`) that can be reused in all the following 42 projects.

The project is a way to understand in depth how these basic functions work (memory, strings,
pointers, dynamic allocation) and to learn how to build and compile a library with a Makefile.
Everything is written following the 42 Norm, and compiles with `-Wall -Wextra -Werror`.

### The library

The library is split into three parts.

#### Part 1 - Libc functions

Reimplementations of standard functions, with the same behaviour and prototype as the originals.

| Function | Description |
|---|---|
| `ft_isalpha` | Checks if a character is a letter |
| `ft_isdigit` | Checks if a character is a digit |
| `ft_isalnum` | Checks if a character is a letter or a digit |
| `ft_isascii` | Checks if a value is an ASCII character (0 to 127) |
| `ft_isprint` | Checks if a character is printable |
| `ft_toupper` | Converts a lowercase letter to uppercase |
| `ft_tolower` | Converts an uppercase letter to lowercase |
| `ft_strlen` | Returns the length of a string |
| `ft_strlcpy` | Copies a string into a buffer of a given size, always NUL-terminated |
| `ft_strlcat` | Appends a string to another within a given buffer size |
| `ft_strchr` | Finds the first occurrence of a character in a string |
| `ft_strrchr` | Finds the last occurrence of a character in a string |
| `ft_strncmp` | Compares up to `n` characters of two strings |
| `ft_strnstr` | Finds a substring within the first `len` characters of a string |
| `ft_atoi` | Converts a string to an `int` |
| `ft_memset` | Fills a memory area with a byte |
| `ft_bzero` | Fills a memory area with zeros |
| `ft_memcpy` | Copies a memory area (areas must not overlap) |
| `ft_memmove` | Copies a memory area, handling overlapping areas |
| `ft_memchr` | Finds a byte in a memory area |
| `ft_memcmp` | Compares two memory areas |
| `ft_calloc` | Allocates memory for an array and sets it to zero |
| `ft_strdup` | Returns a newly allocated copy of a string |

#### Part 2 - Additional functions

Functions that are not in the libc, or that exist in a different form, and that are useful
for string manipulation and output.

| Function | Description |
|---|---|
| `ft_substr` | Returns a new string extracted from another one (start index + length) |
| `ft_strjoin` | Returns a new string made of the concatenation of two strings |
| `ft_strtrim` | Returns a copy of a string with given characters removed from both ends |
| `ft_split` | Splits a string into an array of words using a separator character |
| `ft_itoa` | Converts an `int` to a newly allocated string |
| `ft_strmapi` | Applies a function to each character of a string to build a new string |
| `ft_striteri` | Applies a function to each character of a string, in place |
| `ft_putchar_fd` | Writes a character to a file descriptor |
| `ft_putstr_fd` | Writes a string to a file descriptor |
| `ft_putendl_fd` | Writes a string followed by a newline to a file descriptor |
| `ft_putnbr_fd` | Writes an integer to a file descriptor |

#### Part 3 - Linked lists

Functions to create and manipulate singly linked lists, using the following structure:

```c
typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}	t_list;
```

`content` can point to any kind of data (`void *`), and `next` points to the next node
(`NULL` for the last one).

| Function | Description |
|---|---|
| `ft_lstnew` | Allocates and returns a new node |
| `ft_lstadd_front` | Adds a node at the beginning of a list |
| `ft_lstsize` | Counts the number of nodes in a list |
| `ft_lstlast` | Returns the last node of a list |
| `ft_lstadd_back` | Adds a node at the end of a list |
| `ft_lstdelone` | Frees one node and its content (using a `del` function) |
| `ft_lstclear` | Frees a node and all the following ones, and sets the list to `NULL` |
| `ft_lstiter` | Applies a function to the content of every node |
| `ft_lstmap` | Creates a new list by applying a function to the content of every node |

## Instructions

### Compilation

```bash
make          # builds libft.a
make clean    # removes the object files
make fclean   # removes the object files and libft.a
make re       # fclean + make
```

### Usage

Include the header in your code:

```c
#include "libft.h"
```

Then compile your program with the library:

```bash
cc -Wall -Wextra -Werror main.c -L. -lft -o my_program
```

(`-L.` tells the compiler where `libft.a` is, `-lft` links it. If the library is in another
folder, replace `.` with its path, and add `-I path/to/libft` for the header.)

### Example

```c
#include "libft.h"

int	main(void)
{
	char	**words;
	int		i;

	words = ft_split("hello world from 42", ' ');
	if (!words)
		return (1);
	i = 0;
	while (words[i])
	{
		ft_putendl_fd(words[i], 1);
		free(words[i]);
		i++;
	}
	free(words);
	return (0);
}
```

## Resources

### References

- `man` pages of the original functions (`man strlcpy`, `man memmove`, `man atoi`...), also available online: [man7.org](https://man7.org/linux/man-pages/)
- [cppreference - C language and standard library](https://en.cppreference.com/w/c)
- [Beej's Guide to C Programming](https://beej.us/guide/bgc/) - pointers, memory, structures
- [GeeksforGeeks - Inserting a node in a linked list](https://www.geeksforgeeks.org/c/c-program-for-inserting-a-node-in-a-linked-list/) - course that helped me understand linked lists
- [Apprendre le C - Makefile](https://apprendrelec.com/theory/a22) - course that helped me learn how to write a Makefile
- [Tripouille/libftTester](https://github.com/Tripouille/libftTester) - tester used to check the library (results and memory leaks with valgrind)
- [42-cli](https://github.com/brunogo25/42-cli) - community command-line tool bundling testers for 42 projects, used as a second tester for the library
- The 42 Norm (PDF available on the intranet) and the `norminette` tool

### Acknowledgements

This project was made with the help and explanations of **Luka Hartman** and
**Etienne Lallemand**.

### Use of AI

AI (Claude) was used as a learning and review tool during this project:

- **Debugging**: finding why some files did not compile with `-Wall -Wextra -Werror` (missing prototypes in the header, `const` mismatches with `ft_strlen`, syntax errors) and which lines failed the norminette.
- **Code review**: pointing out bugs in my functions (for example `ft_split`, `ft_atoi`, `ft_putnbr_fd`, `ft_itoa`) and explaining why they were wrong, so I could fix them myself.
- **Explanations**: understanding structures, pointers, double pointers (`t_list **`) and linked lists before writing the linked list functions.
- **Makefile**: the AI explained how a Makefile works and provided one that I analysed line by line.
- **README**: this README was written with the help of the AI.

The functions were written and corrected by me; the AI explained the problems and, in a few
cases (such as `ft_lstadd_front`), showed an example that I studied before writing my own version.
