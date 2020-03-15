clwrap.hpp is a convenient header file for wrapping OpenCL host boilerplate procedure.

After checking out this repository, please source a proper OpenCL
environment first, then make and run it using the OpenCL emulator for a quick functional verification.

Getting Started
---------------

     $ git clone https://github.com/kazutomo/clwrap
     $ cd clwrap

Intel GPU/CPU
-------------

     $ make
     $ ./demohost
     [Platforms]
     0: Portable Computing Language
     1: Intel Gen OCL Driver [selected]
     2: Clover
     3: Experimental OpenCL 2.1 CPU Only Platform
     [Devices]
     Device0: Intel(R) HD Graphics Kabylake Desktop GT1.5 [selected]
     0,1,0 1,1,1 2,1,0 3,1,1 4,1,0 5,1,1 6,1,0 7,1,1 


Emulator (Intel FPGA SDK for OpenCL)
------------------------------------

     $ source /soft/fpga/altera/pro/16.0.2.222/aoc-env.sh
     $ make
     $ ./run-alteraemu.sh
     Kernel: demokernel-emulator.aocx
     Platform0: Altera SDK for OpenCL
     Device0: EmulatorDevice : Emulated Device
     ....
     ....
     inout = 7
     (0,0) (1,1) (2,0) (3,1) (4,0) (5,1) (6,0) (7,1) 

FPGA (using JLSE ruth node)
---------------------------

     $ cp /soft/fpga/altera/clwrap/dummy.aocx .  # use pre-compiled binary
     $ qsub -q fpga_385a -n 1 -t 30 -I
     $ source /soft/fpga/altera/pro/16.0.2.222/aoc-env.sh
     ruth $ ./demohost
     Kernel: demokernel.aocx
     Platform0: Altera SDK for OpenCL
     Device0: p385a_sch_ax115 : nalla_pcie (aclnalla_pcie0)
     Reprogramming device with handle 1
     inout = 7
     (0,0) (1,1) (2,0) (3,1) (4,0) (5,1) (6,0) (7,1) 



NOTE:

- By default, it finds the platform name contains "Intel". The
  platform search keyword can be changed using the CLW_PF environment
  variable.  For example,
  $ CLW_PF=Altera ./yourclwraphostcode



Please feel free to contact me Kaz Yoshii <kazutomo.yoshii@gmail.com> if you have any question.
