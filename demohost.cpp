#include "clwrap.hpp"
#include <sys/time.h>
#include <iomanip>

// A sample code for simpleOCLInit.hpp
// Written by Kaz Yoshii <ky@anl.gov>

// source $OPENCLENV
// g++ -I. -Wall -O2 -g -Wno-unknown-pragmas `aocl compile-config` -std=c++11 -o testclwrap testclwrap.cpp `aocl link-config`
// aocl

using namespace std;

static void test_clwrap()
{
	clwrap  cw;
	bool failed = false;

	cw.info();

	int gsiz = 8;
	int lsiz = 4;

	int *a0 = new int[gsiz];
	int *a1 = new int[gsiz];

	// find demokernel.cl and use 'demokernel' as the kernel name
	bool ret = cw.prepKernel("demokernel");

	if (!ret) {
		cout << "prepKernel() failed!" << endl;
		return;
	}

	cw.appendArg(sizeof(int)*gsiz, a0, cw.DEV2HOST);
	cw.appendArg(sizeof(int)*gsiz, a1, cw.DEV2HOST);

	cw.runKernel(gsiz, lsiz);

	cw.finish();

	// validate the results
	for (int i = 0; i < gsiz; i++) {
		if (a0[i] != i + 10) {
			cout << "Error: a0[" << i << "] should be " << i+10 << ", not " << a0[i] << endl;
			failed = true;
		}
		if (a1[i] != (i%lsiz) + 20) {
			cout << "Error: a1[" << i << "] should be " << (i%lsiz)+20 << ", not " << a1[i] << endl;
			failed = true;
		}
	}
	if (! failed)  cout << "Validation passed" << endl;

	cout << "Elapsed [sec]: " << cw.getKernelElapsedNanoSec() * 1e-9 << endl;

	cout << "[output]" << endl;

	cout << "a0: ";
	for (int i = 0; i < gsiz; i++)
		cout << setw(3) << a0[i];
	cout << endl;
	cout << "a1: ";
	for (int i = 0; i < gsiz; i++)
		cout << setw(3) << a1[i];
	cout << endl;

};

int main()
{
	test_clwrap();

	return 0;
}
