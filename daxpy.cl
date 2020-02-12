// naive daxpy

__kernel void daxpy(double a, 
	 __global const double *restrict x,
         __global double *restrict y)
{
	int g_idx = get_global_id(0);

	y[g_idx] += a * x[g_idx];
}
