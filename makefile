PSPLIBSDIR = $(PSPSDK)/..
TARGET = sime
OBJS = main.o zenkaku.o shinonomefont.o draw.o sime.o

CFLAGS = -G0 -Wall -O2 -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
 
LDFLAGS = -Wl
LIBS = -lpspgu

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Simple IME TEST
PSP_EBOOT_ICON = 

PSPSDK=$(shell psp-config --pspsdk-path)

include $(PSPSDK)/lib/build.mak

PARAM.SFO : $(OBJS)
