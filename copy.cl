// just copy

__kernel void copy(
	 __global const float *restrict from0,
	 __global const float *restrict from1,
	 __global const float *restrict from2,
	 __global const float *restrict from3,
	 __global       float *restrict to0,
	 __global       float *restrict to1,
	 __global       float *restrict to2,
	 __global       float *restrict to3
	 )
{
	int i = get_global_id(0);

	to0[i] = from0[i];
	to1[i] = from1[i];
	to2[i] = from2[i];
	to3[i] = from3[i];
}