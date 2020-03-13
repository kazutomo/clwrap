#include "clwrap.hpp"
#include <sys/time.h>
#include <math.h>

using namespace std;


static void benchslowpi(cl_ulong n, int gsiz, int lsiz)
{
	clwrap  cw;
	int validation_failed = 0;

	cw.listPlatforms();
	cw.listDevices();

	cl_long2 seed;
	seed.s[0] = 123;
	seed.s[1] = 234;


	bool ret = cw.prepKernel("slowpi", "randonly");
	cl_ulong *dummy = new cl_ulong[gsiz];

	cw.appendArg(sizeof(cl_ulong2),  &seed, cw.VALUE);
	cw.appendArg(sizeof(cl_ulong),   &n,    cw.VALUE);
	cw.appendArg(sizeof(cl_ulong)*gsiz, dummy, cw.DEV2HOST);
	cw.runKernel(gsiz, lsiz);

	cout << "* Randonly loop" << endl;
	cout << "Elapsed [Sec]: " << cw.getKernelElapsedNanoSec() * 1e-9 << endl;
	cout << "Rands/sec   : " << (double)gsiz / (cw.getKernelElapsedNanoSec() * 1e-9) << endl;

	cw.clearArgs();

	float *res = new float[gsiz];
	ret = cw.prepKernel("slowpi");
	if (!ret) {
		cout << "prepKernel() failed!" << endl;
		return;
	}

	cw.appendArg(sizeof(cl_ulong2),  &seed, cw.VALUE);
	cw.appendArg(sizeof(cl_ulong),   &n,    cw.VALUE);
	cw.appendArg(sizeof(float)*gsiz, res, cw.DEV2HOST);

	cw.runKernel(gsiz, lsiz);

	cout << "* Slow PI loop" << endl;
	cout << "Elapsed [Sec]: " << cw.getKernelElapsedNanoSec() * 1e-9 << endl;
	cout << "PIs/sec   : " << (double)gsiz / (cw.getKernelElapsedNanoSec() * 1e-9) << endl;



	for (int i = 0; i < gsiz; i++) {
		if ( fabs(res[i] - M_PI) > 0.1 ) {
			cout << res[i] << " ";
			validation_failed ++;
		}
	}

	if (validation_failed == 0) {
		cout << "Validation passed!" << endl;
	} else {
		cout << "Validation failed: " << validation_failed << " out of " << gsiz << endl;
	}
};

int main(int argc, char *argv[])
{
	cl_ulong n = 100000; // niter for PI calculation
	int gsiz = 1024; // # of PI calculated
	int lsiz = 16; // OpenCL localsize

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
