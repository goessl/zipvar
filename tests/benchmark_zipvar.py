from timeit import timeit

setup = """
from collections import deque

def consume(iterator):
    deque(iterator, maxlen=0)

a = range(10_000)
b = range(1_000)
"""
stmt = 'consume(zip_var(a, b))'

N = 1000
time_c = timeit(setup='from zipvar._zipvar import zip_var\n'+setup, stmt=stmt, number=N) / N
time_py = timeit(setup='from zipvar.pyzipvar import zip_var\n'+setup, stmt=stmt, number=N) / N

print(f'C backend average time:      {time_c:.8f} s')
print(f'Python backend average time: {time_py:.8f} s')
print(f'C to Python factor:          {time_py/time_c:.3f}')
