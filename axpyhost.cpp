#include "clwrap.hpp"
#include <sys/time.h>

using namespace std;

#include <sys/time.h>

static double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

static
template <class FPType>
void host_daxpy(int n,
		FPType a,
		FPType *__restrict__ x,
		FPType *__restrict__ y)
{
	int i;
	for (i=0; i<n; i++) y[i] += a * x[i];
}

static
template <class FPType>
void benchdaxpy(int m, int lsiz)
{
	clwrap  cw;

	cw.listPlatforms();
	cw.listDevices();

	int n = 1024*1024 * m;
	int gsiz = n;

	FPType a = 2.0;
	FPType *x = new FPType[n];
	FPType *y = new FPType[n];

	/* init data */
	for (int i = 0; i < n; i++) {
		x[i] = (FPType)i + 1.0;
		y[i] = 0.0;
	}

#ifdef ENABLE_DP
	bool ret = cw.prepKernel("daxpy");
#else
	bool ret = cw.prepKernel("saxpy");
#endif
	if (!ret) {
		cout << "prepKernel() failed!" << endl;
		return;
	}

	cw.appendArg(sizeof(FPType),  &a, cw.VALUE);
	cw.appendArg(sizeof(FPType)*n, x, cw.HOST2DEV);
	cw.appendArg(sizeof(FPType)*n, y, cw.DUPLEX);

	cw.runKernel(gsiz, lsiz);

	cout << "elapsed [nsec]: " << cw.getKernelElapsedNanoSec() << endl;
	cout << "GFlops: " << ((double)n * 2) / cw.getKernelElapsedNanoSec() << endl;

	for (int i = 0; i < n; i++) {
		if (y[i] != x[i] * a) {
			cout << "Validation failed at " << i << " " << y[i] << " " << x[i] << endl;
			exit(1);
		}
	}
	cout << "validation passed!" << endl;

	if (1) {
		double s, e;
		s = gettime();
		host_daxpy <FPType> (n, a, x, y);
		e = gettime() - s;
		cout << "* host axpi\n";
		cout << "elapsed [nsec]: " << e * 1e9 << endl;
		cout << "GFlops: " << ((double)n * 2) / (e * 1e9) << endl;
	}
};

int main(int argc, char *argv[])
{
	int m = 128;
	int lsiz = 256;

	if (argc > 1) {
		m = atoi(argv[1]);
	}
	if (argc > 2) {
		lsiz = atoi(argv[2]);
	}
	printf("Memory %d [MB]\n", m*2*8);
	printf("Localsize = %d\n", lsiz);

#ifdef ENABLE_DP
	benchdaxpy <double>(m, lsiz);
#else
	benchdaxpy <float>(m, lsiz);
#endif
	return 0;
}