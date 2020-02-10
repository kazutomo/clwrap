// A demo OpenCL kernel for testing
// fills global and local index to the global memory array
// Kaz Yoshii <ky@anl.gov>

__kernel void demokernel(__global int *restrict g1,
		    __global int *restrict g2)
{
	int g_idx = get_global_id(0);
	int l_idx = get_local_id(0);

	g1[g_idx] = g_idx + 10;
	g2[g_idx] = l_idx + 20;

#ifdef EMULATOR
	printf("dummy: dim=%d global_size=%2lu global_id=%2lu local_size=%2lu local_id=%2lu num_groups=%2lu group_id=%2lu\n",
	       get_work_dim(),
	       get_global_size(0), get_global_id(0),
	       get_local_size(0), get_local_id(0),
	       get_num_groups(0), get_group_id(0));
#endif
}
