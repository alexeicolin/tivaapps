Overview
========

This is a quick how-to for writing a few hello-world example apps on top of the
Texas Intruments Real-Time Operating System (aka. TI-RTOS with SYSBIOS kernel)
and running and debugging it on the Tiva C Launchpad (TM4C123GXL) evaluation
board ($13) from a Linux host without depending on the IDE from TI.

Of particular interest is the system output re-direction to UART and
workarounds for a few issues in SYSBIOS code.

Example apps:

    - `hello`: console output to UART
    - `hwiswitask`: various threading primitives (ex. from SYSBIOS user manual) 

Most content here was gathered from various existing resources and stitched
together to get an end-to-end working environment. The initiative was started
when there was less support for the Tiva C Launchpad than there is now and
contained more resources (e.g. linker scripts), which are no longer needed and
hence have been removed.

Disclaimer: personal project, not affiliated with TI.

Tool Dependencies
=================

- gcc-arm-none-eabi toolchain
    * Tested with v4-7-2013q3-0precise6 from Ubuntu repos
    * Prefix assumed to be `/usr`

- RTSC XDC Tools v3.25.05.94:
  [standalone distribution](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/rtsc/index.html)
    * Build framework for building XDC packages and applications that comsume
      them. SYSBIOS and TIRTOS are collections of XDC packages.
    * [Overview of RTSC](http://rtsc.eclipse.org/docs-tip/General_Information)
      and
      [an accessible tutorial](http://rtsc.eclipse.org/docs-tip/RTSC_Module_Primer/Lesson_0)
    * Documentation for some packages [here](http://rtsc.eclipse.org/cdoc-tip/index.html).
      The docs for the packages used for the present hello-world are inline in
      the `.xdc` files. They can be compiled into HTML with
      [xdc.tools.cdoc](http://rtsc.eclipse.org/cdoc-tip/index.html#xdc/tools/cdoc/package.html)
      or viewed interactive by setting up
      [xdc.tools.cdoc.sg](http://rtsc.eclipse.org/docs-tip/Command_-_xdc.tools.cdoc)
      viewer (depends on Mozilla).
    * Note: XDC Tools v3.25.00.48 in TI RTOS distribution v1.21.00.09 suffers
      from [Bug 421665](https://bugs.eclipse.org/bugs/show_bug.cgi?id=421665)
      which prevents it from working on 32-bit Linux hosts.

- [OpenOCD](http://openocd.sourceforge.net/) 
    * For flashing and debugging the target via ICDI
    * Tested with v0.7.0 installed from source) 
    * `sudo apt-get install openocd` or from source
    * [Board config script](ek-tm4c123gxl.cfg) included in this repo

Source Dependencies
===================

- TivaWare v2.0.1.11577a: shipped with
  [TI RTOS v1.21.00.09](http://www.ti.com/tool/ti-rtos)
    * Low-level support for microcontroller features.
    * Alternatively, standalone distribution: [SW-TM4C Complete](http://www.ti.com/tool/sw-tm4c)
    * TivaWare 1.0 or 1.1 is also included in the SW-EK-TM4C123GXL distribution,
      which looks superceded by the above
    * To rebuild:

    $ cd tirtos_1_21_00_09/products/TivaWare_C_Series_2.0.1.11577a/
    $ make clean # needed to clean deps files with previous build
    $ make

- ti.sysbios package v6.37.00.20 (patched): [sysbios repo]()
    * Real-time operating system kernel.
    * Note: SYSBIOS suffers from
      [this](http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/p/313785/1097401.aspx#1097401)
      and
      [this](http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/313791.aspx)
      bugs, which break timers. The version in the repo has the patches.
    * Note: This version of SYSBIOS is shipped with TI RTOS distribution
      v1.21.00.09. Current latest version in dedicated SYSBIOS distribution is
      v6.35.04.50, which does not include explicit support for TM4C123, in
      particular default linking scripts. However it can be made to work with a
      hand-written linker script (see earlier commits in this repo).
    * To build:

        $ cd /path/to/sysbios
        $ make -f sysbios.mak XDC_INSTALL_DIR=/path/to/xdctools_3_25_05_94 \
            XDCARGS='gnu.targets.arm.M4F=/usr'

- ti.drivers package: part of
  [TI RTOS](http://www.ti.com/tool/ti-rtos) v1.21.00.09
    * Wraps low-level microcontroller features to be used with XDC
      packages.
    * In [tirtos repo]() repo there are patches to the UART code that
        - remove unneeded interrupts due to the async API, which is unused by
          our app
        - change the UART clock source to PIOSC to inorder to make it robust
          against clock changes due to transitioning into deep sleep (sleeping
          is not enabled by default).
    * To rebuild:

        $ cd /path/to/tirtos_1_21_00_09
        $ make -f tirtos.mak CCS_BUILD=false GCC_INSTALLATION_DIR=/usr \
            TIRTOS_INSTALLATION_DIR=$PWD \
            XDCTOOLS_INSTALLATION_DIR=/path/to/xdctools_3_25_05_94 \
            BIOS_INSTALLATION_DIR=/path/to/sysbios \
            all

- ti.uia.\* packages v1.04.00.06: shipped with
  [TI RTOS](http://www.ti.com/tool/ti-rtos) v1.21.00.09)
    * Instrumentation functionality, of which the Logger framework is used here
    * Rebuilt by tirtos.mak (see ti.drivers item above)

Build and Flash
===============

Setup pointers to the above dependencies

    $ export XDCTOOLS_INSTALLATION_DIR=/path/to/xdctools_3_25_05_94
    $ export TIVAWARE_INSTALLATION_DIR=/path/to/tirtos_1_21_00_09/products/TivaWare_C_Series_2.0.1.11577a/
    $ export BIOS_INSTALLATION_DIR=/path/to/sysbios
    $ export TIRTOS_INSTALLATION_DIR=/path/to/tirtos_1_21_00_09/
    $ export UIA_INSTALLATION_DIR=/path/to/tirtos_1_21_00_09/uia_1_04_00_06

By default all example applications (`app_hello.out` and `app_hwiswitask.out`)
are built.

    $ cd /path/to/tivaapps
    $ make
    $ openocd -f openocd.cfg 'program app_hello.out 0x00000000'

Debug
=====

    $ openocd -f openocd.cfg
    $ gdb
    gdb> target remote localhost:3333
    gdb> monitor reset halt
    gdb> break app
    gdb> c

Add another application
========================

Create an app\_name.c with `app` as the main function, analogous to
`app\_hello.c`. In `Makefile` add the file to `APPS` and add a
corresponding target below `# Applications`.

