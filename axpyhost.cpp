#include "clwrap.hpp"
#include <sys/time.h>
#include <math.h>

using namespace std;

#include <sys/time.h>

static double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

template <class FPType>
static
void host_axpy(int n,
		FPType a,
		FPType *__restrict__ x,
		FPType *__restrict__ y)
{
	int i;
	for (i=0; i<n; i++) y[i] += a * x[i];
}

template <class FPType>
static
void benchaxpy(int m, int lsiz, int niters)
{
	clwrap  cw;

	cw.listPlatforms();
	cw.listDevices();

	int n = (1024 * 1024 * m) / sizeof(FPType);
	int gsiz = n;

	cout << "Number of elems per array : " << n << endl;

	FPType a = 2.0;
	FPType *x = new FPType[n];
	FPType *y = new FPType[n];

	/* init data */
	for (int i = 0; i < n; i++) {
		x[i] = (FPType)(i % 1000) + 1.0;
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

	double elapsed = 0.0;

	cw.writeToDevice();
	for (int j = 0; j < niters; ++j) {
		cw.runKernel(gsiz, lsiz, false);
		elapsed += cw.getKernelElapsedNanoSec();
	}
	cw.readFromDevice();

	// cw.print_timing();

	cout << "elapsed [sec]: " << elapsed * 1e-9 << endl;
	cout << "Device GFlops: " << ((double)n * 2 * niters) / elapsed << endl;
	cout << "Device Mem BW GB/s: " << (sizeof(FPType)*3*n*niters) / elapsed << endl;

	for (int i = 0; i < n; i++) {
		FPType tmp = (x[i] * a) * niters;
		if ( fabs(y[i] - tmp) > 0.00001 ) {
			cout << "Validation failed at " << i << " " << y[i] << " " << tmp <<  " " <<  fabs(y[i] - tmp) << endl;
			exit(1);
		}
	}
	cout << "validation passed!" << endl;

	if (1) {
		double s, e;
		s = gettime();
		for (int j = 0; j < niters; ++j)
			host_axpy <FPType> (n, a, x, y);
		e = gettime() - s;
		cout << "* Host axpi\n";
		cout << "elapsed [sec]: " << e << endl;
		cout << "GFlops: " << ((double)n * 2 *niters) / (e * 1e9) << endl;
	}
};

int main(int argc, char *argv[])
{
	int m = 128;
	int lsiz = 256;
	// int lsiz = 0; // unset
	int n = 2; // # of kernel invocations

	if (argc > 1) {
		m = atoi(argv[1]);
	}
	if (argc > 2) {
		lsiz = atoi(argv[2]);
	}
	if (argc > 3) {
		n = atoi(argv[3]);
	}
	cout << "args: " << m << " " << lsiz << " " << n << endl;
	cout << endl;

	cout << "# of invocations : " << n << endl;
	cout << "Memory [MB]      : " << m   << " (per array)" << endl;
	cout << "Memory [MB]      : " << m*2 << " (total)" << endl;
	if (lsiz > 0)
	    cout << "Localsize        : " << lsiz << endl;
	else
	    cout << "Localsize        : default" << endl;

#ifdef ENABLE_DP
	benchaxpy<double>(m, lsiz, n);
#else
	benchaxpy<float>(m, lsiz, n);
#endif
	return 0;
}
