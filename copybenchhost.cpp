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

static size_t  parse_bufsize_str(const char* str)
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

static void duplineidx(std::vector<std::string> &b, const char *format, int v)
{
    char buf[80];
    snprintf(buf, sizeof(buf), format, v);
    b.push_back(buf);
}
static void duplineidx2(std::vector<std::string> &b, const char *format, int v)
{
    char buf[80];
    snprintf(buf, sizeof(buf), format, v, v);
    b.push_back(buf);
}

static void generatecopycl(const char *fn, int nbuffers)
{
    std::vector<std::string> b;

    b.push_back("// generated by copybenchhost.cpp");
    b.push_back("__kernel void copy(");
    for (int i = 0; i < nbuffers ; ++i) {
	duplineidx(b, "   __global const float *restrict from%d,", i);
    }
    for (int i = 0; i < nbuffers-1 ; ++i) {
	duplineidx(b, "   __global float *restrict to%d,", i);
    }
    duplineidx(b, "   __global float *restrict to%d", nbuffers-1);
    b.push_back(")");
    b.push_back("{");
    b.push_back("   int i = get_global_id(0);");
    for (int i = 0; i < nbuffers ; ++i) {
	duplineidx2(b, "   to%d[i] = from%d[i];", i);
    }
    b.push_back("}");

    ofstream f;
    f.open(fn);
    for(vector<string>::iterator it = b.begin(); it != b.end(); ++it) {
        f << *it << std::endl;
    }
    f.close();
}


static void benchcopy(int nelems, int ninvokes, int lsiz)
{
	clwrap  cw;
	int nbuffers = 4;

	// preparing for a new commandline option that allows users to
	// specify the number of copy buffers.  quick hack. OpenCL can
	// build program from a buffer, but writing to a file is
	// quick. This does not work for FPGA due to very long
	// compilation. anyway, I'll move this bench to other repo
	// later
	generatecopycl("copy.cl", nbuffers);

	cw.listPlatforms();
	cw.listDevices();

	int gsiz = nelems;
	int allocsize = nelems * sizeof(float);

	float **x = new float*[nbuffers];
	float **y = new float*[nbuffers];

	for (int i = 0; i < nbuffers ; ++i ) {
		x[i] = new float[nelems];
		y[i] = new float[nelems];
		/* init data */
		for (int j = 0; j < nelems; j++) {
			x[i][j] = (float)(j % 1000) + 1.0;
			y[i][j] = 0.0;
		}
	}

	bool ret = cw.prepKernel("copy");
	if (!ret) {
		cout << "prepKernel() failed!" << endl;
		return;
	}

	for (int i = 0; i < nbuffers ; ++i ) {
	    cw.appendArg(allocsize, x[i], cw.HOST2DEV);
	}
	for (int i = 0; i < nbuffers ; ++i ) {
	    cw.appendArg(allocsize, y[i], cw.DEV2HOST);
	}

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
	cout << "Device Mem BW GB/s: " << ((float)allocsize * nbuffers * ninvokes) / elapsed << endl;

	for (int i = 0; i < nbuffers; i++) {
		for (int j = 0; j < nelems; j++) {
			float tmp = (float)(j % 1000) + 1.0;

			if (y[i][j] != tmp) {
				cout << "Validation failed at " << i << " ref=" << tmp;
				cout << " y" << i << " " << y[i][j] << std::endl;
				exit(1);
			}
		}
	}
	cout << "validation passed!" << endl;

	for (int i = 0; i < nbuffers ; ++i ) {
	    delete x[i];
	    delete y[i];
	}
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
