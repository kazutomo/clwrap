// naive saxpy

__kernel void daxpy(single a, 
	 __global const single *restrict x,
         __global single *restrict y)
{
	int g_idx = get_global_id(0);

	y[g_idx] += a * x[g_idx];
}
