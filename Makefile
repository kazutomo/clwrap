#PLATFORM = intelfpga
PLATFORM = intelgpu
#PLATFORM = nvidia

ifeq ($(PLATFORM),intelfpga)
#CXX=g++
CXX=arm-linux-gnueabihf-g++
CFLAGS = -Wall -O2 -g -Wno-unknown-pragmas -DINTEL
CFLAGS += $(shell aocl compile-config)
CXXFLAGS = $(CFLAGS) -std=c++11
LDFLAGS = $(shell aocl link-config)
endif

ifeq ($(PLATFORM),intelgpu)
CXX=g++
CXXFLAGS = -Wall -O2 -g -std=gnu++0x -DINTEL
LDFLAGS = -lOpenCL
endif

ifeq ($(PLATFORM),nvidia)
CXX=nvcc
CXXFLAGS = -O2 -std=c++11 -DNVIDIA
LDFLAGS = -lOpenCL
endif

ifneq (,$(SDK))
CXXFLAGS += -I$(SDK)/include

ifneq (,$(ICDHACK))
LDFLAGS += -L$(ICDHACK)/.libs
else
LDFLAGS += -L$(SDK)/lib64
endif

endif

#LDFLAGS = $(shell aocl link-config) -lnalla_pcie_mmd
#LIBC214 = -L/opt/glibc-2.14/lib
#LDFLAGS = $(shell aocl link-config) $(LIBC214)

INSTALL_PATH ?= /usr/local

all: demohost daxpyhost saxpyhost slowpihost copybenchhost

demohost : demohost.cpp clwrap.hpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

daxpyhost : axpyhost.cpp clwrap.hpp
	$(CXX) -o $@ $< -DENABLE_DP $(CXXFLAGS) $(LDFLAGS)

saxpyhost : axpyhost.cpp clwrap.hpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

slowpihost : slowpihost.cpp clwrap.hpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

copybenchhost : copybenchhost.cpp clwrap.hpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)


demokernel.aocx : demokernel.cl
	aoc -march=emulator -DEMULATOR $<

install: clwrap.hpp
	mkdir -p $(INSTALL_PATH)/include
	install -t $(INSTALL_PATH)/include clwrap.hpp

clean:
	rm -f demohost daxpyhost saxpyhost slowpihost *.o

distclean: clean
	rm -f *~
	rm -rf *.aocx *.aoco
