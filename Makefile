M3TOOLS = /usr
XDCPATH = $(SYSBIOS_ROOT)/packages;$(UIA_ROOT)/packages;$(TIRTOS_ROOT)/packages
#CONFIGURO_OPTS = -v --generationOnly
CONFIGURO_OPTS = -v

CONFIGURO = $(XDCTOOLS_ROOT)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro $(CONFIGURO_OPTS)
TARGET = gnu.targets.arm.M4F
PLATFORM = ti.platforms.tiva:TM4C123GH6PM:true
PART=TM4C123GH6PM
ROV_XS_SUFFIX = pm4fg

CONFIG = bld

CC = $(M3TOOLS)/bin/arm-none-eabi-gcc
CFLAGS = -Wall -mcpu=cortex-m4 -mthumb \
		 -mabi=aapcs -mapcs-frame -std=c99 -MD \
		 -DPART_$(PART) -DTIVAWARE \
		 @$(CONFIG)/compiler.opt -O0 -ggdb -I$(TIVAWARE_ROOT)

LD = $(M3TOOLS)/bin/arm-none-eabi-gcc
LFLAGS = -nostartfiles -static -Wl,--gc-sections \
		 -Wl,-T,$(CONFIG)/linker.cmd \
		 -L$(TIVAWARE_ROOT)/driverlib/gcc/ -O0 -g -ldriver

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
	num_format.o \
	event_logger.o \
	main.o \

APPS = \
	app_hello.out \
	app_hwiswitask.out \

all: $(APPS)

# Applications
app_hwiswitask.out : app_hwiswitask.o $(MAIN_OBJS)
app_hello.out : app_hello.o $(MAIN_OBJS)

clean:
	-rm -rf *.o *.out *.d

cfgclean: clean
	-rm -rf *.rov.xs $(CONFIG)
