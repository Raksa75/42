*This activity has been created as part of the 42 curriculum by nzimmerm, binome.*

# push_swap

## Description

`push_swap` sorts a list of integers with two stacks, **a** and **b**, and a limited set of
operations. The program does not print the sorted numbers: it prints the list of operations
that sorts them, one per line, as short as possible.

The program contains four strategies of different complexity, and chooses one at runtime
from the **disorder** of the input, or uses the one given as an option.

### Operations

| Operation | Effect |
|---|---|
| `sa` / `sb` / `ss` | swap the first two elements of a / b / both |
| `pa` / `pb` | move the top of b to a / the top of a to b |
| `ra` / `rb` / `rr` | rotate up: the first element becomes the last |
| `rra` / `rrb` / `rrr` | rotate down: the last element becomes the first |

## Instructions

```bash
make
./push_swap 2 1 3 6 5 8
./push_swap --bench $(shuf -i 0-9999 -n 500) > /dev/null
```

| Option | Strategy |
|---|---|
| *(none)* or `--adaptive` | chosen from the disorder |
| `--simple` | O(n²) |
| `--medium` | O(n√n) |
| `--complex` | O(n log n) |
| `--bench` | prints statistics on stderr after sorting |

Errors (not an integer, outside the `int` range, duplicates, unknown or repeated option)
print `Error` on stderr. Without numbers, nothing is printed.

## Algorithms

All the counts below are numbers of **push_swap operations**, which is the cost model of
the subject.

### Preparation, common to all strategies

- **Normalisation**: each number is replaced by its rank (0 for the smallest, n-1 for the
  largest). The order is the same, so the operations are the same, but every algorithm can
  then work on values from 0 to n-1. The ranks are computed in O(n²) time, without
  printing any operation. Duplicates are detected at the same time.
- **Disorder**: number of pairs (i, j), i before j, with a[i] > a[j], divided by the total
  number of pairs. It is measured before any operation.
- **Already sorted**: nothing is printed.
- **Up to 5 numbers**: the smallest numbers are pushed to b (brought to the top by the
  shortest rotation), the last 3 are sorted in at most 2 operations, then everything comes
  back with `pa`. At most 10 operations for 5 numbers.

### Simple: minimum extraction, O(n²)

While a has more than 3 elements: bring the minimum of a to the top by the shortest way
(`ra` or `rra`), then `pb`. Sort the last 3, then `pa` everything back: the elements come
back in increasing order.

Each extraction costs at most n/2 rotations + 1 push, for n elements: **at most about
n²/2 + 2n operations**, so O(n²). On an almost sorted list, the minimum is usually very
close to the top, and the cost becomes close to 2n.

### Medium: chunks, O(n√n)

1. **Push to b by chunks.** `pushed` counts the elements already sent to b, and
   `width = 1.5 × √n` is the size of the chunk. For the element at the top of a:
   - rank ≤ `pushed`: `pb` then `rb` (the small ones go to the bottom of b);
   - rank ≤ `pushed + width`: `pb`;
   - otherwise: `ra`.
2. **Pull back.** While b is not empty: bring the largest element of b to the top by the
   shortest way, then `pa`.

Complexity: in phase 1, an element is rotated in a only while its rank is outside the
window, and the window moves forward by one at each push. Each element is rotated at most
about n / width times, so at most n × n / (1.5√n), which is O(n√n). In phase 2, b is
almost sorted by chunks: the maximum is always within about `width` positions of the top
or the bottom, so at most n × width, which is also O(n√n).

### Complex: radix sort (LSD), O(n log n)

The ranks are read in binary, from the lowest bit to the highest. For each bit: for each
of the n elements of a, `pb` if the bit is 0, `ra` if it is 1; then `pa` everything back.
After the last bit, a is sorted.

There are ⌈log₂ n⌉ bits, and each pass costs at most 2n operations: **at most
2n × ⌈log₂ n⌉ operations**, so O(n log n). The sort stops as soon as a is sorted.

### Adaptive

| Disorder | Method | Complexity |
|---|---|---|
| < 0.2 | Simple (minimum extraction) | O(n²) |
| 0.2 to < 0.5 | Medium (chunks) | O(n√n) |
| ≥ 0.5 | Complex (radix) | O(n log n) |

**Why these thresholds** (they are the ones required by the subject; here is why they make
sense for these methods):

- **Low disorder (< 0.2)**: most elements are already close to their place. The minimum
  to extract is usually a few positions from the top, so minimum extraction costs close to
  n operations, much less than its n²/2 worst case, and less than radix, which always
  pays its 2n log n.
- **Medium disorder (0.2 to 0.5)**: the list is mixed enough for minimum extraction to
  become expensive, but the chunk method stays efficient and gives the best results on
  real sizes (100 to 500 numbers).
- **High disorder (≥ 0.5)**: the order is close to random or reversed. Radix does not
  depend on the order at all: its cost is the same for any input, with a guaranteed
  O(n log n) bound.

The comparison is done with integers (`mistakes × 5 < pairs` for 0.2,
`mistakes × 2 < pairs` for 0.5), so there is no rounding problem.

### Memory

Two arrays of n `int` (stacks a and b) and a 4 KB output buffer: **O(n) space** for all
strategies. The ranks are computed in place, using b as a temporary array.

## Performance

Measured on 10 random lists for each case (number of operations):

| Strategy | 100 numbers (average / max) | 500 numbers (average / max) |
|---|---|---|
| `--medium` | 577 / 595 | 5,063 / 5,203 |
| `--complex` | 1,084 / 1,084 | 6,784 / 6,784 |
| default (adaptive) | between medium and complex, max 1,084 | between medium and complex, max 6,784 |
| `--simple` | 1,488 / 1,582 | 32,427 / 33,233 |

The adaptive strategy uses the chunks when a random list has a disorder just under 0.5,
and the radix when it is just over: its worst case is the radix one, which is under 1,500
for 100 numbers and under 8,000 for 500 numbers.

## Files

```
PushSwap/
├── Makefile
├── README.md
├── includes/push_swap.h   # structures, operations, prototypes
└── srcs/
    ├── main.c             # loading, strategy, output
    ├── parse.c            # options and number of values
    ├── parse_numbers.c    # reading the integers, errors
    ├── stack.c            # allocation, normalisation, disorder
    ├── stack_ops.c        # the operations on one stack
    ├── ops.c              # the 11 operations: apply, print, count
    ├── utils.c            # min / max position, shortest rotation, square root
    ├── print.c            # strcmp, output to a file descriptor
    ├── bench.c            # --bench
    ├── sort_small.c       # 2 to 5 numbers
    ├── sort_simple.c      # O(n²)
    ├── sort_medium.c      # O(n√n)
    ├── sort_complex.c     # O(n log n)
    └── adaptive.c         # choice from the disorder
```

## Contributions

| Part | nzimmerm | binome |
|---|---|---|
| Parsing and errors | | |
| Stacks and operations | | |
| Disorder and `--bench` | | |
| Small sorts | | |
| Simple / Medium / Complex | | |
| Adaptive, README, tests | | |

## Resources

- [Big O notation](https://en.wikipedia.org/wiki/Big_O_notation)
- [Radix sort](https://en.wikipedia.org/wiki/Radix_sort)
- [Inversion (discrete mathematics)](https://en.wikipedia.org/wiki/Inversion_(discrete_mathematics)),
  the idea behind the disorder metric

**AI usage**: to be completed (what was asked to an AI, what was checked and rewritten).
