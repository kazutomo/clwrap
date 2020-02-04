clwrap.hpp is a convenient header file for wrapping OpenCL host boilerplate procedure.

After checking out this repository, please source a proper OpenCL
environment first, then make and run it using the OpenCL emulator for a quick functional verification.

Getting Started
---------------

     $ git clone https://github.com/kazutomo/clwrap
     $ cd clwrap


* Intel GPU/CPU

     $ make
     $ ./testclwrap
     [Platforms]
     0: Portable Computing Language
     1: Intel Gen OCL Driver [selected]
     2: Clover
     3: Experimental OpenCL 2.1 CPU Only Platform
     [Devices]
     Device0: Intel(R) HD Graphics Kabylake Desktop GT1.5 [selected]
     0,1,0 1,1,1 2,1,0 3,1,1 4,1,0 5,1,1 6,1,0 7,1,1 


* Emulator (Intel FPGA SDK for OpenCL)

     $ source /soft/fpga/altera/pro/16.0.2.222/aoc-env.sh
     $ make
     $ ./run-alteraemu.sh
     Kernel: dummy-emulator.aocx
     Platform0: Altera SDK for OpenCL
     Device0: EmulatorDevice : Emulated Device
     ....
     ....
     inout = 7
     (0,0) (1,1) (2,0) (3,1) (4,0) (5,1) (6,0) (7,1) 

* FPGA (using JLSE ruth node)

     $ cp /soft/fpga/altera/clwrap/dummy.aocx .  # use pre-compiled binary
     $ qsub -q fpga_385a -n 1 -t 30 -I
     $ source /soft/fpga/altera/pro/16.0.2.222/aoc-env.sh
     ruth $ ./testclwrap
     Kernel: dummy.aocx
     Platform0: Altera SDK for OpenCL
     Device0: p385a_sch_ax115 : nalla_pcie (aclnalla_pcie0)
     Reprogramming device with handle 1
     inout = 7
     (0,0) (1,1) (2,0) (3,1) (4,0) (5,1) (6,0) (7,1) 



NOTE:

- Only tested it with OpenCL in Quartus 16.0.2.222 and Nallatech 385A BSP for 16.0.2.222

- Please reserve a node using qsub when you need to compile your
  kernel targetting actual FPGA board, not emulator. Quartus
  compilation is a heavy task; running multiple compilers slow login
  nodes down.

- To measure the elapsed time runs on an actual FPGA board , call
  cw.getKernelElapsedNanoSec() after cw.runKernel().

- To read the power consumption, uncomment the following line in
  clwrap.hpp, call cw.readboardpower() from host and recompile it

  //#define ENABLE_AOCL_MMD_HACK

   Note: you need to write your version of cw.runKernel() if you want
   to sample power while your kernel is running.

Please feel free to contact me Kaz Yoshii <kazutomo.yoshii@gmail.com> if you have any question.
