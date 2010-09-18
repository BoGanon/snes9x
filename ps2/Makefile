SUBDIRS =

#CPU_SHUTDOWN=1
#SYS_CONFIG_FILE=1
#DEBUG=1
#MOVIE_SUPPORT=1
#JMA_SUPPORT=1
#DEBUGGER=1
#LOGGER_SUPPORT=1
#CONFFILE_SUPPORT=1
#NETPLAY_SUPPORT=1
HAVE_LIBPNG=1
ZLIB=1
UNZIP_SUPPORT=1
SDD1_DECOMP=1

SNES9X_FLAGS = -DHAVE_LIBPNG -DZLIB -DCORRECT_VRAM_READS -DSDD_DECOMP -DUNZIP_SUPPORT -fno-inline -ffast-math
SNES9X_INCS = -I.. -I../apu -I../unzip

SNES9X_OBJS = \
    ../fxinst.o \
    ../fxemu.o \
    ../fxdbg.o


SNES9X_OBJS += \
    ../c4.o \
    ../c4emu.o

# APU
SNES9X_OBJS += \
    ../apu/apu.o \
    ../apu/SNES_SPC.o \
    ../apu/SNES_SPC_misc.o \
    ../apu/SNES_SPC_state.o \
    ../apu/SPC_DSP.o \
    ../apu/SPC_Filter.o

# DSP
SNES9X_OBJS += \
    ../dsp.o \
    ../dsp1.o \
    ../dsp2.o \
    ../dsp3.o \
    ../dsp4.o 

# Base
SNES9X_OBJS += \
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

# ASMCPU Doesn't exist anymore.
SNES9X_OBJS += \
    ../cpuops.o \
    ../cpuexec.o \
    ../sa1cpu.o

SNES9X_OBJS += \
    ../cheats.o \
    ../cheats2.o

# Zip support is nonconfigurable.
SNES9X_OBJS += \
    ../loadzip.o \
    ../unzip/unzip.o \
    ../unzip/ioapi.o \
    ../unzip/zip.o

ifdef DEBUGGER
SNES9X_OBJS += ../debug.o
endif

ifdef LOGGER_SUPPORT
SNES9X_OBJS += ../logger.o
endif

ifdef CONFFILE_SUPPORT
SNES9X_OBJS += ../conffile.o
SNES9X_OBJS += ../snes9x.o
endif

ifdef MOVIE_SUPPORT
SNES9X_OBJS += ../movie.o
endif

ifdef SDD1_DECOMP
SNES9X_OBJS += ../sdd1emu.o
endif

ifdef NETPLAY_SUPPORT
SNES9X_OBJS += \
    ../netplay.o \
    ../server.o
endif

ifdef JMA_SUPPORT
SNES9X_LDFLAGS = libjma.a
SNES9X_OBJS += \
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

INTERFACE_OBJS = \
    interface/src/cfg.o \
    interface/src/gui.o \
    interface/src/gzip.o \
    interface/src/hdd.o \
    interface/src/init.o \
    interface/src/paths.o \
    interface/src/tar.o

INTERFACE_INCS = \
    -Iinterface/include

EE_BIN = snes9x.elf

EE_OBJS = ps2_snes9x.o snes9x_cfg.o $(INTERFACE_OBJS) $(SNES9X_OBJS)

EE_CFLAGS = $(SNES9X_FLAGS)
EE_CXXFLAGS = $(SNES9X_FLAGS) -fno-exceptions -fno-rtti

EE_INCS = $(INTERFACE_INCS) $(SNES9X_INCS) -I$(PS2SDK)/ports/include
EE_LDFLAGS = -L$(PS2SDK)/ports/lib

EE_GFX_LIBS = -ldma -lpacket -lgraph -lfont -ldraw -limage
EE_FILE_LIBS = -lfileXio
EE_SYS_LIBS = -lmc -lmtap -lpadx -lpoweroff -lpatches -lmf -lc

EE_LIBS = $(EE_GFX_LIBS) $(EE_PAD_LIBS) -lconfig -lpng -lz $(EE_FILE_LIBS) $(EE_SYS_LIBS)

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

include Makefile.eeglobal
include Makefile.pref
