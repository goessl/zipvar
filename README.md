# zipvar

The awkward middle child between `zip` and `zip_longest`.
Not short. Not long. Just right.
```python
>>> from zipvar import zip_var
>>> a = (1, 2, 3)
>>> b = [4, 5, 6, 7, 8]
>>> c = set()
>>> for x in zip_var(a, b, c):
...     print(x)
...
(1, 4)
(2, 5)
(3, 6)
(7,)
(8,)
```

## Installation

```console
pip install git+https://github.com/goessl/zipvar.git
```

## Usage

One module `zipvar` with a single class `zipvar`.

- `zip_var(*iterators)`: Return a `zip_var` object whose `.__next__()` method
returns tuples containing one element from each of the given iterables. The
order of elements in each tuple always matches the order of the input
iterables. Iteration continues until all iterables are exhausted, then a
StopIteration is raised. Once an iterable is exhausted, it is excluded from
the resulting tuples - they become shorter over time.

## todo

 - [ ] as C extension

## License (MIT)

Copyright (c) 2025 Sebastian Gössl

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
