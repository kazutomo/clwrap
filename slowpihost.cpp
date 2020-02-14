#include "clwrap.hpp"
#include <sys/time.h>

using namespace std;


static void benchslowpi(cl_ulong n, int gsiz, int lsiz)
{
	clwrap  cw;

	cw.listPlatforms();
	cw.listDevices();

	float *res = new float[gsiz];

	bool ret = cw.prepKernel("slowpi");
	if (!ret) {
		cout << "prepKernel() failed!" << endl;
		return;
	}

	cl_long2 seed;
	seed.s[0] = 123;
	seed.s[1] = 234;
	
	cw.appendArg(sizeof(cl_ulong2),  &seed, cw.VALUE);
	cw.appendArg(sizeof(cl_ulong),   &n,    cw.VALUE);
	cw.appendArg(sizeof(float)*gsiz, res, cw.DEV2HOST);

	cw.runKernel(gsiz, lsiz);

	cout << "Elapsed [Sec]: " << cw.getKernelElapsedNanoSec() * 1e-9 << endl;
	cout << "Loops/sec   : " << (double)n / (cw.getKernelElapsedNanoSec() * 1e-9) << endl;

	for (int i = 0; i < 4 /*gsiz*/; i++) {
		cout << res[i] << endl;
	}
};

int main(int argc, char *argv[])
{
	cl_ulong n = 100000;
	int gsiz = 256;
	int lsiz = 8;

	if (argc > 1) {
		n = atoi(argv[1]);
	}
	if (argc > 2) {
		gsiz = atoi(argv[2]);
	}
	if (argc > 3) {
		lsiz = atoi(argv[3]);
	}
	printf("niters     = %lu\n", n);
	printf("globalsize = %d\n",  gsiz);
	printf("localsize  = %d\n",  lsiz);

	benchslowpi(n, gsiz, lsiz);

	return 0;
}
