#M3TOOLS ?= /home/jdoe/cs/arm-2011q3
#SYSBIOS ?= /home/jdoe/bios_6_33_00_19
#XDCTOOLS ?= /home/jdoe/xdctools_3_23_00_32

M3TOOLS = /usr
SYSBIOS_ROOT = /home/acolin/rtml/src/ti/src/sysbios
XDCPATH = /home/acolin/rtml/src/ti/repo;$(SYSBIOS_ROOT)/packages;$(UIA_ROOT)/packages;$(TIRTOS_ROOT)/packages
#CONFIGURO_OPTS = -v --generationOnly
CONFIGURO_OPTS = -v

CONFIGURO = $(XDCTOOLS_ROOT)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro $(CONFIGURO_OPTS)
TARGET = gnu.targets.arm.M4
PLATFORM = ti.platforms.stellaris:TM4C123GH6PM
PART=TM4C123GH6PM
ROV_XS_SUFFIX = pm4g

CONFIG = bld

CC = $(M3TOOLS)/bin/arm-none-eabi-gcc
CFLAGS = -Wall -mcpu=cortex-m4 -mthumb \
		 -mabi=aapcs -mapcs-frame -std=c99 -MD \
		 -DPART_$(PART) -DTIVAWARE \
		 @$(CONFIG)/compiler.opt -O0 -ggdb -I$(TIVAWARE_ROOT)

LD = $(M3TOOLS)/bin/arm-none-eabi-gcc
LFLAGS = -nostartfiles -static -Wl,--gc-sections \
		 -Wl,-T,$(LINKERCMD) -Wl,-T,$(CONFIG)/linker.cmd \
		 -L$(TIVAWARE_ROOT)/driverlib/gcc/ -O0 -g -ldriver

#LINKERCMD = lm3s9b90.ld
LINKERCMD = tm4c123gxl.ld

.PRECIOUS: %/compiler.opt %/linker.cmd

%/compiler.opt %/linker.cmd : %.cfg
	$(CONFIGURO) -c $(M3TOOLS) -t $(TARGET) -p $(PLATFORM) -r debug $<
	cp $*/package/cfg/$*_$(ROV_XS_SUFFIX).rov.xs .

%.o : %.c

%.o : %.c $(CONFIG)/compiler.opt
	$(CC) $(CFLAGS) -c $<

%.out : %.o $(CONFIG)/linker.cmd
	$(CC) $(CFLAGS) -o $@ $(filter %.o %.a, $^) $(LFLAGS)

MAIN_OBJS = \
	EK_TM4C123GXL.o \
	uart_iface.o \
	event_logger.o \
	main.o \

APPS = \
	app_hwiswitask.out \

all: $(APPS)

# Applications
app_hwiswitask.out : app_hwiswitask.o $(MAIN_OBJS)

clean:
	-rm -rf *.o *.out *.d

cfgclean: clean
	-rm -rf *.rov.xs $(CONFIG)
