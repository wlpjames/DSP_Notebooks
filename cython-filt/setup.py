from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import numpy
import os

os.environ['CFLAGS'] = '-O3 -Wall -std=c++11 -stdlib=libc++'
setup(ext_modules=[
	  Extension("IIR_filt", ["IIR_filt.pyx", "c_python_test_filt.cpp"], language="c++",)],
      cmdclass = {'build_ext': build_ext}, include_dirs=[numpy.get_include()],
       extra_compile_args=['-stdlib=libc++'], language_level=3)

'''
setup(
    ext_modules = cythonize("IIR_filt.pyx")
)
'''