
vecadd: ocl_vector_addition.cpp vector_add_kernel.cl
	clang++ $< -framework OpenCL -std=c++11 -o $@

clean: 
	rm vecadd
