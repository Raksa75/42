*This activity has been created as part of the 42 curriculum by nzimmerm.*

# ft_printf

## Description

**ft_printf** is a 42 common core project. The goal is to recode the `printf()` function from
the C standard library, and to gather it in a static library (`libftprintf.a`) that can be
reused in the following projects.

The project is a way to learn how **variadic functions** work in C (functions that take a
variable number of arguments, using `va_start`, `va_arg` and `va_end`), and to practise number
base conversion and recursion. Everything is written following the 42 Norm, and compiles with
`-Wall -Wextra -Werror`. The function uses my own [libft](../libft).

### Prototype

```c
int	ft_printf(const char *input, ...);
```

Like the original, `ft_printf` returns the **number of characters printed**.

### Supported conversions

| Conversion | Prints | Example |
|---|---|---|
| `%c` | A single character | `ft_printf("%c", 'A')` → `A` |
| `%s` | A string (`(null)` if the pointer is `NULL`) | `ft_printf("%s", "hello")` → `hello` |
| `%p` | A pointer address in hexadecimal, prefixed with `0x` (`(nil)` if `NULL`) | `ft_printf("%p", ptr)` → `0x7ffd5e8a1b2c` |
| `%d` | A signed decimal (base 10) number | `ft_printf("%d", -42)` → `-42` |
| `%i` | A signed integer in base 10 | `ft_printf("%i", 42)` → `42` |
| `%u` | An unsigned decimal (base 10) number | `ft_printf("%u", -1)` → `4294967295` |
| `%x` | A number in hexadecimal, lowercase | `ft_printf("%x", 255)` → `ff` |
| `%X` | A number in hexadecimal, uppercase | `ft_printf("%X", 255)` → `FF` |
| `%%` | A percent sign | `ft_printf("%%")` → `%` |

### How it works

- `ft_printf` reads the format string character by character. Normal characters are printed as they are; when a `%` is found, the next character tells which type of argument to read with `va_arg`.
- A separate function selects the right conversion and returns the number of characters is printed, so `ft_printf` only has to add them up.
- `%d`, `%i` and `%u` use a recursive function working on a `long`, which can hold every `int` and every `unsigned int`.
- `%x`, `%X` and `%p` use the same base 16 conversion function; `%p` only adds the `0x` prefix before it.
- As required by the subject, the buffer management of the original `printf()` is not implemented: every character is written directly with `write`.

## Instructions

### Compilation

```bash
make          # builds libftprintf.a
make clean    # removes the object files
make fclean   # removes the object files and libftprintf.a
make re       # fclean + make
```

### Usage

Include the header in your code:

```c
#include "ft_printf.h"
```

Then compile your program with the library:

```bash
cc -Wall -Wextra -Werror main.c -L. -lftprintf -o my_program
```

### Example

```c
#include "ft_printf.h"

int	main(void)
{
	int	count;

	count = ft_printf("%s is %d years old, %x in hex\n", "Nico", 20, 20);
	ft_printf("%d characters printed\n", count);
	return (0);
}
```

## Resources

### References

- `man 3 printf` - behaviour and return value of the original function
- `man 3 stdarg` - documentation of `va_start`, `va_arg`, `va_copy` and `va_end`
- [cppreference - Variadic functions](https://en.cppreference.com/w/c/variadic)
- [42-cli](https://github.com/brunogo25/42-cli) - community command-line tool bundling testers for 42 projects
- The 42 Norm (PDF available on the intranet) and the `norminette` tool

### Acknowledgements

This project was made with the help and explanations of **Etienne Lallemand**.

### Use of AI

AI (Claude) was used as a learning and review tool during this project:

- **Explanations**: a course on variadic functions (`va_start`, `va_arg`, `va_copy`, `va_end`), and explanations on how `printf` knows the number of arguments
- **Debugging**: finding why some files did not compile with `-Wall -Wextra -Werror` (missing prototypes in the header, variable names, types) and which lines failed the norminette.
- **Code review**: pointing out bugs in my functions (base conversion)
- **README**: this README was written with the help of the AI.x
