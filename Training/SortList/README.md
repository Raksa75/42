# Training

Exercises to train for the **42 exams**: small functions written in limited time, without
the internet, and tested with my own `main`.

Each exercise has its own folder, with a `Makefile` that builds a test program.

| Exercise | Description | Status |
|---|---|---|
| [sort_list](./SortList) | Sort a linked list, in the order given by a comparison function | ✅ Done |

## sort_list

Sorts a linked list of `int`, using a comparison function given as a parameter. The function
returns `0` if two values are in the wrong order. The same function can then sort in
ascending or descending order.

```c
typedef struct s_list
{
	int				data;
	struct s_list	*next;
}	t_list;

void	sort_list(t_list **lst, int (*f)(int a, int b));
```

The values are swapped between the nodes (the links are not changed): when two neighbours
are in the wrong order, they are swapped, and the check starts again from the beginning of
the list.

### Tests

```bash
cd SortList
make
./test
```

`main.c` checks fixed cases (ascending, descending, duplicates, negative numbers, one
element, already sorted, empty list), then 10,000 random lists: each one must be sorted
and keep exactly the same values. Checked without leaks with `valgrind`.

## Files

```
Training/
├── README.md
└── SortList/
    ├── Makefile
    ├── list.h         # t_list and the prototype
    ├── sort_list.c    # the exercise
    └── main.c         # the tests
```
