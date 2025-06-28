from setuptools import Extension, setup

setup(
    packages = ['zipvar'],
    ext_modules = [
        Extension(
            name = 'zipvar._zipvar',
            sources = ['zipvar/_zipvarmodule.c'],
            py_limited_api = True
        )
    ]
)
