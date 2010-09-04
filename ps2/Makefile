SUBDIRS =

#JMA=1
#DEBUGGER=1
#LOGGER=1
#SNES_OBJ=1
#NETPLAY=1
HAVE_LIBPNG=1
ZLIB=1
UNZIP_SUPPORT=1
SDD1_DECOMP=1

SNES9X_FLAGS = -DHAVE_LIBPNG -DZLIB -DCORRECT_VRAM_READS -DSDD_DECOMP -DUNZIP_SUPPORT -fno-inline -ffast-math
EE_BIN = snes9x.elf
EE_CFLAGS = $(SNES9X_FLAGS)
EE_CXXFLAGS = $(SNES9X_FLAGS) -fno-exceptions -fno-rtti

EE_INCS = -I.. -I../apu -I../unzip -I$(PS2SDK)/ports/include
EE_LDFLAGS = -L$(PS2SDK)/ports/lib
EE_LIBS = -lpng -lz -lmf -lc

EE_OBJS = ps2_snes9x.o paths/paths.o 

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
    ../sdd1.o \
    ../tile.o \
    ../srtc.o \
    ../gfx.o \
    ../memmap.o \
    ../clip.o \
    ../ppu.o \
    ../dma.o \
    ../globals.o \
    ../reader.o \
    ../bsx.o \
    ../snapshot.o \
    ../screenshot.o \

ifdef DEBUGGER
EE_OBJS += ../debug.o
endif

ifdef LOGGER
EE_OBJS += ../logger.o
endif

ifdef SNES9X_OBJ
EE_OBJS += ../snes9x.o
endif

ifdef CONFFILE
EE_OBJS += ../conffile.o
endif

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

ifdef JMA
EE_LDFLAGS = libjma.a
EE_OBJS += \
    ../jma/s9x-jma.o \
    ../jma/7zlzma.o \
    ../jma/crc32.o \
    ../jma/iiostrm.o \
    ../jma/inbyte.o \
    ../jma/jma.o \
    ../jma/lzma.o \
    ../jma/lzmadec.o \
    ../jma/winout.o
#EE_CXXFLAGS = -fexceptions
endif

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include Makefile.eeglobal
include Makefile.pref
