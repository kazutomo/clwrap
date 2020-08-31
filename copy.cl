// just copy

__kernel void copy(
	 __global const double *restrict from0,
	 __global const double *restrict from1,
	 __global const double *restrict from2,
	 __global const double *restrict from3,
	 __global       double *restrict to0,
	 __global       double *restrict to1,
	 __global       double *restrict to2,
	 __global       double *restrict to3
	 )
{
	int i = get_global_id(0);

	to0[i] = from0[i];
	to1[i] = from1[i];
	to2[i] = from2[i];
	to3[i] = from3[i];
}
