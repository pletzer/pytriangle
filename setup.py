#!/usr/bin/env python

"""
To use, type:
python setup.py build
python setup.py install
python setup.py install --prefix=...
python setup.py bdist --format=wininst
python setup.py bdist --format=rpm
python setup.py sdist --formats=gztar,zip
python setup.py register
python setup.py sdist upload -r pypi
"""

import sys

if not hasattr(sys, 'version_info') or sys.version_info < (2,6,0,'',0):
    raise SystemExit("Python 2.7 or later required to build triangle!")


from distutils.core import setup, Extension

setup (name = "pytriangle",
       version = "1.0.7",
       description='A 2D triangulation program originally written by Jonathan Richard Shewchuck',
       url="http://github.com/pletzer/pytriangle",
       py_modules = ["triangle",],
       author="Alexander Pletzer",
       author_email="alexander@gokliya.net",
       ext_modules = [Extension("triangulate", # name of the shared library
                                          ["code/triangle.c", 
                                           "code/trianglemodule.c"],
                                define_macros=[("TRILIBRARY",1),
                                               ("NO_TIMER",1)],
                                include_dirs=["code",],
                                ),
                      
                      ]
       )
