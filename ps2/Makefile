SUBDIRS =

#CPU_SHUTDOWN=1
#SYS_CONFIG_FILE=1
#DEBUG=1
#MOVIE_SUPPORT=1
JMA_SUPPORT=1
#DEBUGGER=1
#LOGGER_SUPPORT=1
#CONFFILE_SUPPORT=1
#NETPLAY_SUPPORT=1
HAVE_LIBPNG=1
ZLIB=1
UNZIP_SUPPORT=1
SDD1_DECOMP=1

PS2SDK_FLAGS = -DNEW_PADMAN -UPS2LINK -fshort-double -mhard-float -ffast-math -mno-check-zero-division
SNES9X_FLAGS = $(PS2SDK_FLAGS) -DRIGHTSHIFT_IS_SAR -DDEBUG -DZLIB -DHAVE_LIBPNG -DSDD1_DECOMP -DUNZIP_SUPPORT -DJMA_SUPPORT

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
EE_CXXFLAGS = -fexceptions
else
EE_CXXFLAGS = -fno-exceptions
endif


INTERFACE_OBJS = \
    interface/src/browser.o \
    interface/src/cfg.o \
    interface/src/gui.o \
    interface/src/gzip.o \
    interface/src/hdd.o \
    interface/src/init.o \
    interface/src/interface.o \
    interface/src/lists.o \
    interface/src/paths.o \
    interface/src/settings.o \
    interface/src/strkat.o \
    interface/src/tar.o \
    interface/src/video.o

INTERFACE_INCS = \
    -Iinterface/include

EE_BIN = snes9x.elf

EE_OBJS = ps2_snes9x.o modules_tgz.o skin_tgz.o snes9x_cfg.o ps2_controls.o ps2_video.o ps2_sound_driver_audsrv.o $(INTERFACE_OBJS) $(SNES9X_OBJS)

EE_CFLAGS = $(SNES9X_FLAGS)
EE_CFLAGS += -fno-builtin-memcmp -fno-builtin-memcpy -fno-builtin-memset
EE_CFLAGS += -fno-builtin-printf -fno-builtin-strcat -fno-builtin-strchr
EE_CFLAGS += -fno-builtin-strcmp -fno-builtin-strcpy -fno-builtin-strlen
EE_CFLAGS += -fno-builtin-strncpy -fno-builtin-strrchr -fno-builtin-strncmp

EE_CXXFLAGS += $(SNES9X_FLAGS) -fno-rtti -fno-implicit-inline-templates

EE_INCS = $(INTERFACE_INCS) $(SNES9X_INCS) -I$(PS2SDK)/ports/include
EE_LDFLAGS = -L$(PS2SDK)/ports/lib

EE_GFX_LIBS = -ldma -lpacket -lgraph -lfont -ldraw -limage
EE_FILE_LIBS = -lfileXio
EE_SYS_LIBS = -lSMSCDVD -laudsrv -linputx -lmc -lmtap -lpadx -lpoweroff -lpatches -lmf -lc -lm

EE_LIBS = $(EE_GFX_LIBS) $(EE_PAD_LIBS) -lconfig -lpng -lz $(EE_FILE_LIBS) $(EE_SYS_LIBS)

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN)

%_tgz.s:
	bin2s $*.tgz $*_tgz.s $*_tgz

include Makefile.eeglobal
include Makefile.pref
