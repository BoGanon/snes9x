#include <snes9x.h>
#include <display.h>

int main()
{
	return 0;
}

bool8 S9xOpenSnapshotFile (const char *filepath, bool8 read_only, STREAM *file)
{
	return 0;
}

void S9xCloseSnapshotFile (STREAM file)
{
}

void S9xExit (void)
{
}

bool8 S9xInitUpdate (void)
{
	return 0;
}

bool8 S9xDeinitUpdate (int width, int height)
{
	return 0;
}

void S9xMessage (int type, int number, const char *message)
{
}

bool8 S9xOpenSoundDevice (void)
{
	return 0;
}

const char *S9xGetFilename (const char *extension, enum s9x_getdirtype dirtype)
{
	return NULL;
}

const char *S9xGetFilenameInc (const char *extension, enum s9x_getdirtype dirtype)
{
	return NULL;
}

const char *S9xGetDirectory (enum s9x_getdirtype dirtype)
{
	return NULL;
}

const char *S9xChooseFilename (bool8 read_only)
{
	return NULL;
}

const char *S9xChooseMovieFilename (bool8 read_only)
{
	return NULL;
}

const char *S9xBasename (const char *path)
{
	return NULL;
}

void S9xAutoSaveSRAM (void)
{
}
void S9xToggleSoundChannel (int c)
{
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
	return 0;
}
