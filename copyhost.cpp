// (setq c-basic-offset 8)

#include <sys/time.h>
#include <math.h>
#include <sys/time.h>

#include "clwrap.hpp"

using namespace std;

static double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

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

	if ( m > 1 ) v = std::stoi(a.substr(0, a.length()-1)) * m;
	else         v = std::stoi(a);
	return v;
}

void benchcopy(int nelems, int ninvokes, int lsiz)
{
	clwrap  cw;

	cw.listPlatforms();
	cw.listDevices();

	int gsiz = nelems;
	int allocsize = nelems * sizeof(float);

	float *x0 = new float[nelems];
	float *x1 = new float[nelems];
	float *x2 = new float[nelems];
	float *x3 = new float[nelems];
	float *y0 = new float[nelems];
	float *y1 = new float[nelems];
	float *y2 = new float[nelems];
	float *y3 = new float[nelems];

	/* init data */
	for (int i = 0; i < nelems; i++) {
		x0[i] = x1[i] = x2[i] = x3[i] = (float)(i % 1000) + 1.0;
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

	float elapsed = 0.0;

	double host_st, host_et;

	host_st = gettime();
	cw.writeToDevice();
	for (int j = 0; j < ninvokes; ++j) {
		cw.runKernel(gsiz, lsiz, false);
		elapsed += cw.getKernelElapsedNanoSec();
	}
	cw.readFromDevice();
	host_et = gettime() - host_st;

	cw.print_timing();

	std::printf("elapsed [sec]: %.7f # host timer\n", host_et);
	std::printf("elapsed [sec]: %.7f # device timer\n", elapsed * 1e-9);
	cout << "Device Mem BW GB/s: " << ((float)allocsize * 4.0 * ninvokes) / elapsed << endl;

	for (int i = 0; i < nelems; i++) {
		float tmp = (float)(i % 1000) + 1.0; 
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
	cout << argv[0] << " " << nelems << " " << ninvokes << endl;
	cout << "# of invocations : " << ninvokes << endl;
	// cout << "# of elems       : " << nelems << endl;
	// cout << "Memory [Bytes]   : " << nelems*sizeof(float) << " # per array" << endl;
	cout << "Memory [Bytes]   : " << nelems*sizeof(float)*4 << " # total" << endl;
	if (lsiz > 0)
		cout << "Localsize        : " << lsiz << endl;
	else
		cout << "Localsize        : default" << endl;

	benchcopy(nelems, ninvokes, lsiz);

	return 0;
}
