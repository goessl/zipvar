"""zipvar package

Provides the `zip_var` iterator object, implemented in C for speed,
falling back to a pure Python version if the C extension is unavailable.
"""

try:
    from ._zipvar import zip_var
except ImportError as e:
    import warnings
    warnings.warn(
        "C extension _zipvar not available. Falling back to pure Python implementation (slower):" + str(e),
        ImportWarning,
        stacklevel = 2
    )
    from .pyzipvar import zip_var
