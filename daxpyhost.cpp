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

static void host_daxpy(int n,
		       double a,
		       double *__restrict__ x,
		       double *__restrict__ y)
{
	int i;
	for (i=0; i<n; i++) y[i] += a * x[i];
}


static void benchdaxpy(int m, int lsiz)
{
	clwrap  cw;

	cw.listPlatforms();
	cw.listDevices();

	int n = 1024*1024 * m;
	int gsiz = n;

	double a = 2.0;
	double *x = new double[n];
	double *y = new double[n];

	/* init data */
	for (int i = 0; i < n; i++) {
		x[i] = (double)i + 1.0;
		y[i] = 0.0;
	}

	bool ret = cw.prepKernel("daxpy");
	if (!ret) {
		cout << "prepKernel() failed!" << endl;
		return;
	}

	cw.appendArg(sizeof(double),  &a, cw.VALUE);
	cw.appendArg(sizeof(double)*n, x, cw.HOST2DEV);
	cw.appendArg(sizeof(double)*n, y, cw.DUPLEX);

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
		host_daxpy(n, a, x, y);
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

	benchdaxpy(m, lsiz);

	return 0;
}
