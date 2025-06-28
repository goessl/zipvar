"""Pure Python implementation of the zipvar module."""

def zip_var(*iterables):
    """Return a `zip_var` object whose `.__next__()` method returns tuples
    containing one element from each of the given iterables.
    
    The order of elements in each tuple always matches the order of the input
    iterables. Iteration continues until all iterables are exhausted, then a
    StopIteration is raised. Once an iterable is exhausted, it is excluded from
    the resulting tuples - they become shorter over time.
    """
    iterators = list(map(iter, reversed(iterables)))
    while iterators:
        result = []
        for i in reversed(range(len(iterators))):
            try:
                result.append(next(iterators[i]))
            except StopIteration:
                del iterators[i]
        if result:
            yield tuple(result)
