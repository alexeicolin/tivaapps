MAIN_OBJS = \
	EK_TM4C123GXL.o \
	uart_iface.o \
	num_format.o \
	event_logger.o \
	syscalls.o \
	delay.o \
	main.o \

APPS = \
	app_hello.out \
	app_hwiswitask.out \
	app_exception.out \
	app_button.out \
	app_pwm.out \
	app_timer.out \
	app_clock.out \
	app_busy.out \

HEADERS = \
	Board.h \
	EK_TM4C123GXL.h \
	time.h \
	delay.h \
	event_logger.h \
	num_format.h \
	uart_iface.h \

# Can't make all apps since using target-specific variables
all: app_hello.out

# Applications with target-specific variables and extra deps
app_button.out : CFLAGS += -DBUTTON_HANDLER_MASK=0x3
app_button.out : wakeup_button.o
app_hello.out : CFLAGS += -DBUTTON_HANDLER_MASK=0x2
app_hello.out : wakeup_button.o
app_busy.out : CFLAGS += -DNO_UART

ifndef BIOS_INSTALLATION_DIR
$(error Environment variable not defined: BIOS_INSTALLATION_DIR)
endif
ifndef UIA_INSTALLATION_DIR
$(error Environment variable not defined: UIA_INSTALLATION_DIR)
endif
ifndef TIRTOS_INSTALLATION_DIR
$(error Environment variable not defined: TIRTOS_INSTALLATION_DIR)
endif
ifndef TIVAWARE_INSTALLATION_DIR
$(error Environment variable not defined: TIVAWARE_INSTALLATION_DIR)
endif

M3TOOLS = /usr
XDCPATH_LIST = \
	$(BIOS_INSTALLATION_DIR)/packages \
	$(UIA_INSTALLATION_DIR)/packages \
	$(TIRTOS_INSTALLATION_DIR)/packages \

TARGET = gnu.targets.arm.M4F
PLATFORM = ti.platforms.tiva:TM4C123GH6PM:true
PART=TM4C123GH6PM
ROV_XS_SUFFIX = pm4fg

#CONFIGURO_OPTS = -v --generationOnly
CONFIGURO_OPTS = -v

empty:=
space:= $(empty) $(empty)
XDCPATH = $(subst $(space),;,$(XDCPATH_LIST))

CONFIGURO = $(XDCTOOLS_INSTALLATION_DIR)/xs --xdcpath="$(XDCPATH)" \
			xdc.tools.configuro $(CONFIGURO_OPTS)

CONFIG = bld

CC = $(M3TOOLS)/bin/arm-none-eabi-gcc
CFLAGS = -Wall -mcpu=cortex-m4 -mthumb \
		 -mabi=aapcs -mapcs-frame -std=c99 -MD \
		 -DPART_$(PART) -DTIVAWARE \
		 @$(CONFIG)/compiler.opt -O0 -ggdb -I$(TIVAWARE_INSTALLATION_DIR)

LD = $(M3TOOLS)/bin/arm-none-eabi-gcc
LFLAGS = -nostartfiles -static -Wl,--gc-sections \
		 -Wl,-T,$(CONFIG)/linker.cmd \
		 -L$(TIVAWARE_INSTALLATION_DIR)/driverlib/gcc/ -O0 -g -ldriver

.PRECIOUS: %/compiler.opt %/linker.cmd

%/compiler.opt %/linker.cmd : %.cfg
	$(CONFIGURO) -c $(M3TOOLS) -t $(TARGET) -p $(PLATFORM) -r debug $<
	cp $*/package/cfg/$*_$(ROV_XS_SUFFIX).rov.xs .

%.o : %.c

%.o : %.c $(CONFIG)/compiler.opt
	$(CC) $(CFLAGS) -c $<

%.out : %.o $(CONFIG)/linker.cmd $(MAIN_OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(filter %.o %.a, $^) $(LFLAGS)

clean:
	-rm -rf *.o *.out *.d

cfgclean: clean
	-rm -rf *.rov.xs $(CONFIG)
