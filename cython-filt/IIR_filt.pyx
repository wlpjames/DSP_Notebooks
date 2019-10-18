 
# distutils: language = c++

#from IIR_filt cimport IIR_filter

# import both numpy and the Cython declarations for numpy
from cython cimport view
import numpy as np
cimport numpy as np


cdef extern from "c_python_test_filt.cpp" namespace "filters":
	cdef cppclass IIR_filter:
		#public functions
		IIR_filter() except +
		IIR_filter(const char*, int, float, float) except +
		int process(float*, int)
		void set_coefs(float*, float*)

cdef class PyIIR_filt:
	cdef IIR_filter *thisptr      # hold a C++ instance which we're wrapping
	
	def __cinit__(self, const char* t, int order, float cutoff, float slope):
		self.thisptr = new IIR_filter(t, order, cutoff, slope)
	
	def __dealloc__(self):
		del self.thisptr
	
	def process(self, np.ndarray[double, mode="c"] signal, int siglen):
		cdef float* s = <float*> signal.data
		
		self.thisptr.process(<float*>s, siglen)

		s_view = <float[:siglen]>s
		
		return np.asarray(s_view)

	def set_coefs(self, np.ndarray[double, mode="c"] B, np.ndarray[double, mode="c"] A):
		print B
		print A
		cdef float* b = <float*> B.data
		cdef float* a = <float*> A.data

		for i in range(3):
			print b[i]
			print a[i]

		return self.thisptr.set_coefs(b, a)


#%%cython --compile-args=-stdlib=libc++ --link-args=-stdlib=libc++ --cplus