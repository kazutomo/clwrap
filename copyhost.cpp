#include "clwrap.hpp"
#include <sys/time.h>
#include <math.h>

// (setq c-basic-offset 8)

using namespace std;

#include <sys/time.h>

size_t  parse_bufsize_str(const char* str)
{
	if (!str) return -1;
	std::string a(str);

	size_t v = 0;
	size_t m = 1;    
	if( a.length() > 1 ) {

		int c = toupper(a.back());
		switch(c) {
		case 'K':  m = 1 << 10; break;
		case 'M':  m = 1 << 20; break;
		case 'G':  m = 1 << 30; break;
		}
	}

	if ( m > 1 ) v = std::stoi(a.substr(0,a.length()-1));
	else         v = std::stoi(a);
	return v;
}

void benchcopy(int nelems, int ninvokes, int lsiz)
{
	clwrap  cw;

	cw.listPlatforms();
	cw.listDevices();

	int gsiz = nelems;
	int allocsize = nelems * sizeof(double);

	double *x0 = new double[nelems];
	double *x1 = new double[nelems];
	double *x2 = new double[nelems];
	double *x3 = new double[nelems];
	double *y0 = new double[nelems];
	double *y1 = new double[nelems];
	double *y2 = new double[nelems];
	double *y3 = new double[nelems];

	/* init data */
	for (int i = 0; i < nelems; i++) {
		x0[i] = x1[i] = x2[i] = x3[i] = (double)(i % 1000) + 1.0;
		y0[i] = y1[i] = y2[i] = y3[i] = 0.0;
	}

	bool ret = cw.prepKernel("copy");
	if (!ret) {
		cout << "prepKernel() failed!" << endl;
		return;
	}

	cw.appendArg(allocsize, x0, cw.HOST2DEV);
	cw.appendArg(allocsize, x1, cw.HOST2DEV);
	cw.appendArg(allocsize, x2, cw.HOST2DEV);
	cw.appendArg(allocsize, x3, cw.HOST2DEV);
	cw.appendArg(allocsize, y0, cw.DEV2HOST);
	cw.appendArg(allocsize, y1, cw.DEV2HOST);
	cw.appendArg(allocsize, y2, cw.DEV2HOST);
	cw.appendArg(allocsize, y3, cw.DEV2HOST);

	double elapsed = 0.0;

	cw.writeToDevice();
	for (int j = 0; j < ninvokes; ++j) {
		cw.runKernel(gsiz, lsiz, false);
		elapsed += cw.getKernelElapsedNanoSec();
	}
	cw.readFromDevice();

	cw.print_timing();

	cout << "elapsed [sec]: " << elapsed * 1e-9 << endl;
	cout << "Device Mem BW GB/s: " << ((double)allocsize * 4.0 * ninvokes) / elapsed << endl;

	for (int i = 0; i < nelems; i++) {
		double tmp = (double)(i % 1000) + 1.0; 
		if ( y0[i] != tmp || y1[i] != tmp || y2[i] != tmp || y3[i] != tmp ) {
			cout << "Validation failed at " << i << " ref=" << tmp;
			cout << " y0" << y0[i];
			cout << " y1" << y1[i];
			cout << " y2" << y2[i];
			cout << " y3" << y3[i] << std::endl;
			exit(1);
		}
	}
	cout << "validation passed!" << endl;
}

int main(int argc, char *argv[])
{
	int nelems = 1024*1024;
	int ninvokes = 8;
	int lsiz = 0;
    
	if (argc > 1) {
		nelems = parse_bufsize_str(argv[1]);
	}
	if (argc > 2) {
		ninvokes = atoi(argv[2]);
	}
    
	cout << "# of elems       : " << nelems << endl;
	cout << "# of invocations : " << ninvokes << endl;
	cout << "Memory [Bytes]   : " << nelems*sizeof(double)  << endl;
	if (lsiz > 0)
		cout << "Localsize        : " << lsiz << endl;
	else
		cout << "Localsize        : default" << endl;

	benchcopy(nelems, ninvokes, lsiz);

	return 0;
}
