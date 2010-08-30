SUBDIRS =

SDD1_DECOMP=1

EE_BIN = snes9x.elf
EE_CXXFLAGS = -DSDD_DECOMP -fno-inline -fno-exceptions -fno-rtti

EE_INCS = -I.. -I$(PS2SDK)/ports/include
EE_LDFLAGS = -L$(PS2SDK)/ports/lib
EE_LIBS = -lz -lc

EE_OBJS = needc.o needcpp.o paths/paths.o 

EE_OBJS += \
    ../fxinst.o \
    ../fxemu.o \
    ../fxdbg.o


EE_OBJS += \
    ../c4.o \
    ../c4emu.o

# APU
EE_OBJS += \
    ../apu/apu.o \
    ../apu/SNES_SPC.o \
    ../apu/SNES_SPC_misc.o \
    ../apu/SNES_SPC_state.o \
    ../apu/SPC_DSP.o \
    ../apu/SPC_Filter.o

# DSP
EE_OBJS += \
    ../dsp.o \
    ../dsp1.o \
    ../dsp2.o \
    ../dsp3.o \
    ../dsp4.o 

# Base
EE_OBJS += \
    ../spc7110.o \
    ../obc1.o \
    ../seta.o \
    ../seta010.o \
    ../seta011.o \
    ../seta018.o \
    ../controls.o \
    ../crosshairs.o \
    ../cpu.o \
    ../sa1.o \
    ../debug.o \
    ../sdd1.o \
    ../tile.o \
    ../srtc.o \
    ../gfx.o \
    ../memmap.o \
    ../clip.o \
    ../ppu.o \
    ../dma.o \
    ../snes9x.o \
    ../globals.o \
    ../reader.o \
    ../conffile.o \
    ../bsx.o \
    ../logger.o \
    ../snapshot.o \
    ../screenshot.o \

ifdef MOVIE
EE_OBJS += ../movie.o
endif

# ASMCPU Doesn't exist anymore.
EE_OBJS += \
    ../cpuops.o \
    ../cpuexec.o \
    ../sa1cpu.o

EE_OBJS += \
    ../cheats.o \
    ../cheats2.o

ifdef SDD1_DECOMP
EE_OBJS += ../sdd1emu.o
endif

ifdef NETPLAY
EE_OBJS += \
    ../netplay.o \
    ../server.o
endif

# Zip support is nonconfigurable.
EE_OBJS += \
    ../loadzip.o \
    ../unzip/unzip.o \
    ../unzip/ioapi.o \
    ../unzip/zip.o

UNZIPDEFINES=-DUNZIP_SUPPORT

ifdef JMA
EE_LDFLAGS = libjma.a
EE_OBJS = \
    ../jma/s9x-jma.o \
    ../jma/7zlzma.o \
    ../jma/crc32.o \
    ../jma/iiostrm.o \
    ../jma/inbyte.o \
    ../jma/jma.o \
    ../jma/lzma.o \
    ../jma/lzmadec.o \
    ../jma/winout.o
EE_CXXFLAGS = -fexceptions
endif

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS)

include Makefile.eeglobal
include Makefile.pref
