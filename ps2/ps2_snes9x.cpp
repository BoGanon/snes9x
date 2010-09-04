// Standard headers
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

// ps2sdk headers
#include <fileXio_rpc.h>

// Ported headers
#include <zlib.h>

// Settings
#if 0
#include <settings.h>
#endif

#include <paths.h>

// Snes9x headers
#include <snes9x.h>
#include <display.h>
#include <memmap.h>
#include <apu.h>
#include <controls.h>
#include <conffile.h>


unsigned int palette[256];

// miscellaneous paths
static char misc[PATH_MAX + 1];

// Needs to be global
#if 0
settings_t *settings;
#endif

int main(int argc, char **argv)
{
	return 0;
}

// conffile.h
void S9xParsePortConfig(ConfigFile &, int pass)
{
	
}

// controls.h
void S9xHandlePortCommand (s9xcommand_t cmd, int16 data1, int16 data2)
{
	return;
}

bool S9xPollButton (uint32 id, bool *pressed)
{
	return true;
}

bool S9xPollPointer (uint32 id, int16 *x, int16 *y)
{
	return true;
}

bool S9xPollAxis (uint32 id, int16 *value)
{
	return true;
}
bool8 S9xOpenSnapshotFile (const char *filepath, bool8 read_only, STREAM *file)
{

	STREAM getfile = *file;

	if ((filepath == NULL) || (file == NULL))
	{
		return 0;
	}

	if (read_only)
	{
		getfile = gzopen(filepath,"r");
	}
	else
	{
		getfile = gzopen(filepath,"w6h");
	}

	if (getfile == Z_NULL)
	{
		return 0;
	}
	else
	{
		return 1;
	}

	return 0;

}

void S9xCloseSnapshotFile (STREAM file)
{
	gzclose(file);
}

void S9xExit (void)
{
	//longjmp(somewhere,buf);
}

bool8 S9xInitUpdate (void)
{
	return 1;
}

bool8 S9xDeinitUpdate (int width, int height)
{
	return 1;
}

void S9xMessage (int type, int number, const char *message)
{
	printf("%s\n",message);
}

bool8 S9xOpenSoundDevice (void)
{
	return 1;
}

const char	dir_types[13][32] =
{
	"",				// DEFAULT_DIR
	"",				// HOME_DIR
	"",				// ROMFILENAME_DIR
	"rom",			// ROM_DIR
	"sram",			// SRAM_DIR
	"savestate",	// SNAPSHOT_DIR
	"screenshot",	// SCREENSHOT_DIR
	"spc",			// SPC_DIR
	"cheat",		// CHEAT_DIR
	"patch",		// IPS_DIR
	"bios",			// BIOS_DIR
	"log",			// LOG_DIR
	""				// LAST_DIR
};

#if 0
void make_s9x_dirs(void)
{
	int i;
	const char *path;
	iox_stat_t stat;
	int filemode =  FIO_S_IRUSR | FIO_S_IWUSR | FIO_S_IXUSR | FIO_S_IRGRP | FIO_S_IWGRP | FIO_S_IXGRP | FIO_S_IROTH | FIO_S_IWOTH | FIO_S_IXOTH;

	for (i = 0; i < LAST_DIR; i++)
	{

		if (i == HOME_DIR)
		{
			continue;
		}
		else if (i == ROMFILENAME_DIR)
		{
			continue;
		}

		path = S9xGetDirectory(i);

		if (fileXioGetStat(path,&stat) < 0)
		{
			fileXioMkDir(path,filemode);
		}

	}

}
#endif

const char *S9xGetDirectory (enum s9x_getdirtype dirtype)
{
#if 0
	strcpy(misc,settings->home);

	if (dir_types[dirtype][0])
	{
		strcat(misc,"/");
		strcat(misc,dir_types[dirtype]);
	}
#endif
	return misc;

}

const char *S9xGetFilename (const char *extension, enum s9x_getdirtype dirtype)
{

	// directory
	char directory[PATH_MAX+1];

	// components
	char drive[10];
	char dir[PATH_MAX+1];
	char fname[PATH_MAX+1];
	char ext[PATH_MAX+1];

	strcpy(directory,S9xGetDirectory(dirtype));
	_splitpath(Memory.ROMFilename, drive, dir, fname, ext);

	snprintf(misc, PATH_MAX + 1, "%s%s%s%s", directory, SLASH_STR, fname, extension);

	return misc;

}

const char *S9xGetFilenameInc (const char *extension, enum s9x_getdirtype dirtype)
{
#if 0
	iox_stat_t stat;
	int i = 0;

	char directory[PATH_MAX+1];
	char drive[10];
	char dir[PATH_MAX+1];
	char fname[PATH_MAX+1];
	char ext[PATH_MAX+1];

	strcpy(directory,S9xGetDirectory(dirtype));
	_splitpath(Memory.ROMFilename, drive, dir, fname, ext);

	do
	{
		snprintf(misc, PATH_MAX + 1, "%s%s%s.%03d%s", directory, SLASH_STR, fname, i++, extension);

		if (i > 100)
		{
			break;
		}
	}
	while (!fileXioGetStat(misc, &stat));
#endif
	return misc;

}

const char *S9xChooseFilename (bool8 read_only)
{
	return NULL;
}

#ifdef MOVIE
const char *S9xChooseMovieFilename (bool8 read_only)
{
	return NULL;
}
#endif

const char *S9xBasename (const char *path)
{
	char base[256];
	strcpy(base,path);

	return basename(base);
}

void S9xAutoSaveSRAM (void)
{
	Memory.SaveSRAM(S9xGetFilename(".srm", SRAM_DIR));
}

void S9xToggleSoundChannel (int c)
{
	static unsigned char sound_switch = 255;

	if (c == 8)
		sound_switch = 255;
	else
		sound_switch ^= 1 << c;

	S9xSetSoundControl(sound_switch);
}

void S9xSetPalette (void)
{
}

void S9xSyncSpeed (void)
{
}

// gfx.h
bool8 S9xContinueUpdate (int width, int height)
{
	return 1;
}
