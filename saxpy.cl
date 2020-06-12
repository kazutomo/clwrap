// naive saxpy

__kernel void saxpy(float a,
	 __global const float *restrict x,
         __global float *restrict y)
{
	int g_idx = get_global_id(0);

	y[g_idx] += a * x[g_idx];
}
