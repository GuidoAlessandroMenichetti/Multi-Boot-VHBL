TARGET = MULTIBOOT
OBJS = main.o utils.o

CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBS = -lz -lm 
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = MULTIBOOT

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
