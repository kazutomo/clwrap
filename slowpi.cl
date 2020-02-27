// slow PI cacllation, a fake compute intensive kernel


ulong xorshift128plus(ulong2* s)
{
	ulong x = (*s).x;
        ulong const y = (*s).y;
        (*s).x = y;
        x ^= x << 23; 
        (*s).y = x ^ y ^ (x >> 17) ^ (y >> 26); 
        return (*s).y + y;
}


__kernel void randonly(
	 ulong2 seed,
	 const ulong n,
	 __global ulong *restrict res)
{
	int g_idx = get_global_id(0);
	ulong tmp = 0;
	ulong dist = 10000000;
	ulong i;

	seed.x += g_idx;

	for (i = 0; i < n; i++) {
	     tmp = tmp ^ (xorshift128plus(&seed) % dist);
	}

	res[g_idx] = tmp;
}


__kernel void slowpi(
	 ulong2 seed,
	 const ulong n,
	 __global float *restrict res)
{
	int g_idx = get_global_id(0);
	ulong cnt = 0;
	ulong dist = 10000000;
	float x, y;
	ulong i;

	seed.x += g_idx;

	for (i = 0; i < n; i++) {
	    x = (float)(xorshift128plus(&seed) % dist);
	    y = (float)(xorshift128plus(&seed) % dist);
	    if ( sqrt(x*x + y*y) < (float)dist ) cnt ++;
	}

	res[g_idx] = 4.0 * (float)cnt / (float)n;
}
