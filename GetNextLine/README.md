*This activity has been created as part of the 42 curriculum by nzimmerm.*

# get_next_line

## Description

**get_next_line** is a 42 common core project. The goal is to write a function that returns
one line read from a file descriptor each time it is called:

```c
char	*get_next_line(int fd);
```

Repeated calls (for example in a loop) read the whole file, one line at a time. The function
works with regular files as well as with the standard input.

- The returned line includes the terminating `\n`, except for the last line of a file that does not end with `\n`.
- The function returns `NULL` when there is nothing left to read, or if an error occurred.
- The returned line is allocated with `malloc` and must be freed by the caller.

The project is a way to learn how `read()` and file descriptors work, how **static variables**
keep data between function calls, and how to manage memory carefully. Everything is written
following the 42 Norm, and compiles with `-Wall -Wextra -Werror`.

## Instructions

### Compilation

The project has no Makefile: compile the files with your own program, and choose the buffer
size used by `read()` with the `-D BUFFER_SIZE=n` option:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

If `BUFFER_SIZE` is not given, a default value of 42 is used.

### Example

```c
#include <fcntl.h>
#include <stdio.h>
#include "get_next_line.h"

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("test.txt", O_RDONLY);
	if (fd == -1)
		return (1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

### Files

| File | Content |
|---|---|
| `get_next_line.c` | `get_next_line()` and the reading loop |
| `get_next_line_utils.c` | Helper functions: string length, `\n` search, join, line extraction, rest of the reserve |
| `get_next_line.h` | Prototypes and default `BUFFER_SIZE` |

## Algorithm

### Overview

The function keeps a **reserve**: a string stored in a `static` variable, which contains
everything that was read from the file but not yet returned. Each call works in four steps:

1. **Read until a line is complete.** As long as the reserve does not contain a `\n`, call `read()` to get `BUFFER_SIZE` more bytes and append them to the reserve. Stop as soon as a `\n` is found, or when `read()` returns 0 (end of file).
2. **Check if there is something to return.** If the reserve is empty, there is no line left: free it and return `NULL`.
3. **Extract the line.** Copy the beginning of the reserve, up to and including the first `\n` (or the whole reserve if there is no `\n`), into a new string.
4. **Keep the rest.** Replace the reserve with what follows the `\n`, so it is used at the next call.

If `read()` returns -1, the reserve is freed and the function returns `NULL`.

### Example

File content `"abcd\nefgh\nij"`, `BUFFER_SIZE=5`:

| Call | `read()` | Reserve after reading | Returned line | Reserve kept |
|---|---|---|---|---|
| 1 | `"abcd\n"` | `"abcd\n"` | `"abcd\n"` | `""` |
| 2 | `"efgh\n"` | `"efgh\n"` | `"efgh\n"` | `""` |
| 3 | `"ij"`, then 0 | `"ij"` | `"ij"` | `NULL` |
| 4 | 0 | empty | `NULL` | `NULL` |

### Justification

- **A static variable for the reserve.** `read()` does not know about lines: one call can return part of a line, or several lines at once. What was read but not returned yet must survive until the next call. A `static` local variable keeps its value between calls, without using a global variable (which is forbidden by the subject).
- **Reading as little as possible.** The function stops reading as soon as the reserve contains a `\n`. It never reads the whole file at once, as required by the subject: if a line is already in the reserve, no `read()` is done at all.
- **The read buffer is allocated with `malloc`.** A local array of `BUFFER_SIZE` bytes would be stored on the stack, which is limited (8 MB by default on Linux): with `BUFFER_SIZE=10000000`, the program would crash. Allocating the buffer on the heap avoids this limit, and it is freed right after reading.
- **New strings at each step.** Joining, extracting and keeping the rest always create a new string of the exact size needed, and free the old one. This keeps the memory management simple and avoids leaks.
- **Trade-off.** Each `read()` copies the whole reserve into a new string. With a very small `BUFFER_SIZE` and very long lines, this means many copies; in exchange, the code stays short and easy to check.

## Resources

### References

- `man 2 read` - behaviour and return values of `read()`
- `man 2 open` and `man 2 close` - opening and closing files (used in the test program)
- [cppreference - Storage duration (static variables)](https://en.cppreference.com/w/c/language/storage_duration)
- [42-cli](https://github.com/brunogo25/42-cli) - community command-line tool bundling testers for 42 projects
- The 42 Norm (PDF available on the intranet) and the `norminette` tool

### Use of AI

AI (Claude) was used as a learning and review tool during this project:

- **Explanations**: a course on file descriptors, `read()`, `open()`, `BUFFER_SIZE` and static variables, and explanations of the reading logic (what to keep between calls, how to detect the end of a line and of the file).
- **Code review**: pointing out bugs in my functions (out-of-bounds writes, wrong malloc sizes, `NULL` reserve on the first call, freed pointer kept in the static variable) and explaining why they were wrong.
- **README**: this README was written with the help of the AI.
