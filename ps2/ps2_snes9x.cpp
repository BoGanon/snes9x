// C++ standard headers
#include <iostream>

// C Standard headers
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

// ps2sdk headers
#include <input.h>
#include <audsrv.h>
#include <fileXio_rpc.h>
#include <dma.h>
#include <packet.h>
#include <gs_psm.h>
#include <graph.h>
#include <draw.h>

// Ported headers
#include <zlib.h>
#include <libconfig.h>

// Interface headers
#include <browser.h>
#include <init.h>
#include <interface.h>
#include <paths.h>
#include <settings.h>

// Snes9x headers
#include <snes9x.h>
#include <display.h>
#include <memmap.h>
#include <apu.h>
#include <controls.h>

#include <snes9x_cfg.h>
#include <ps2_controls.h>
#include <ps2_video.h>
#include <ps2_sound_driver_audsrv.h>

int stop_emulation = 0;

S9xSoundDriver *driver;

void make_s9x_dirs();

bool8 S9xInitSoundDevice(void);
void S9xDeinitSoundDevice(void);

void S9xSyncSpeedFinish();

void S9xInit()
{
	char *path;

	ps2_input_open_pads();

	path = check_boot("snes9x.cfg");

	S9xLoadCFG(path);

	ps2_input_cfg(path);

	S9xInitGFX();

	S9xInitSoundDevice();

	S9xReportControllers();

#ifdef GFX_MULTI_FORMAT
	S9xSetRenderPixelFormat(BGR555);
#endif

	if (!S9xGraphicsInit())
	{
		printf("Out of Memory\n");
	}

}

void S9xDeinit()
{

	browser_reset_path();

	stop_emulation = 0;

	S9xGraphicsDeinit();

	S9xDeinitGFX();

	S9xDeinitSoundDevice();

	ps2_input_close_pads();

}

int main(int argc, char **argv)
{
	int loaded = 0;
	unsigned long current;
	unsigned long last = 0;
	char fps_str[20];
	int frames = 0;

	std::ios::sync_with_stdio(false);

	parse_args(argc,argv);

	init("snes9x.cfg");
	init_sound_modules(NULL);

	S9xInitSettings();

	make_s9x_dirs();

	if (!Memory.Init () || !S9xInitAPU ())
	{
		printf("Out of Memory\n");
	}

	while (1)
	{

		interface_open();

		interface_run();

		interface_close();

		S9xInit();

		printf("rom = %s\n",browser_get_path());

		loaded = Memory.LoadROM(browser_get_path());

		if (loaded)
		{
			printf("Loading SRAM: %s\n", S9xGetFilename(".srm", SRAM_DIR));
			printf("loaded = %d\n",Memory.LoadSRAM(S9xGetFilename(".srm", SRAM_DIR)));
		}
		else
		{
			stop_emulation = 1;
			S9xReset();
		}

		while (!stop_emulation)
		{

			if (Settings.DisplayFrameRate)
			{
				current = (unsigned int)(((float)clock()) / 576000.0f);

				if (current != last)
				{
					sprintf(fps_str,"fps = %d",frames);
					S9xSetInfoString (fps_str);
					frames = 0;
					last = current;
				}

				frames++;

			}

			S9xMainLoop();

			ps2_input_report_pads();

		}

		printf("Saving SRAM: %s\n",S9xGetFilename(".srm",SRAM_DIR));
		printf("saved = %d\n",Memory.SaveSRAM(S9xGetFilename(".srm", SRAM_DIR)));

		S9xDeinit();

	}

	return 0;
}

#ifdef CONFFILE_SUPPORT
// conffile.h
void S9xParsePortConfig(ConfigFile &, int pass)
{
	return;
}
#endif

// controls.h
void S9xHandlePortCommand (s9xcommand_t cmd, int16 data1, int16 data2)
{
	return;
}

bool S9xPollButton (uint32 id, bool *pressed)
{
	return false;
}

bool S9xPollPointer (uint32 id, int16 *x, int16 *y)
{
	return false;
}

bool S9xPollAxis (uint32 id, int16 *value)
{
	return false;
}

bool8 S9xOpenSnapshotFile (const char *filepath, bool8 read_only, STREAM *file)
{

	STREAM getfile = *file;

	if ((filepath == NULL) || (file == NULL))
	{
		return FALSE;
	}

	if (read_only)
	{
		getfile = OPEN_STREAM(filepath,"r");
	}
	else
	{
#ifdef ZLIB
		getfile = OPEN_STREAM(filepath,"w6h");
#else
		getfile = OPEN_STREAM(filepath,"w");
#endif
	}

	if ((getfile == Z_NULL) || (getfile == NULL))
	{
		return FALSE;
	}

	return TRUE;

}

void S9xCloseSnapshotFile (STREAM file)
{
	gzclose(file);
}

void S9xExit (void)
{
	stop_emulation = 1;
}

bool8 S9xInitUpdate (void)
{
	return TRUE;
}

void S9xMessage (int type, int number, const char *message)
{
	printf("%s\n",message);
}

bool8 S9xInitSoundDevice(void)
{

	driver = new S9xAudsrvSoundDriver();

	if (driver != NULL)
	{

		driver->init();

		// hardcoded buffer_ms, lag_ms
		S9xInitSound(96,0);

		S9xSetSoundMute(FALSE);

	}
	else
	{

		S9xSetSoundMute(TRUE);

	}

	return TRUE;
}

void S9xDeinitSoundDevice(void)
{
	driver->terminate();
	delete driver;
}

bool8 S9xOpenSoundDevice (void)
{

	return driver->open_device ();

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


void make_s9x_dirs(void)
{

	int i;

	for (i = 0; i < LAST_DIR; i++)
	{

		// DEFAULT_DIR is the same as these
		if (i == HOME_DIR)
		{
			continue;
		}
		else if (i == ROMFILENAME_DIR)
		{
			continue;
		}

		fileXioMkdir(S9xGetDirectory((s9x_getdirtype)i),0666);

	}

}

const char *S9xGetDirectory (enum s9x_getdirtype dirtype)
{
	static char directory[PATH_MAX+1];
	settings_t settings = settings_get();

	// Initialize directory with base directory
	strcpy(directory,settings.home.directory);

	// Make path for subdirectories
	if (dir_types[dirtype][0])
	{
		sprintf(directory,"%s/%s",settings.home.directory,dir_types[dirtype]);
	}

	printf("directory = %s\n", directory);

	return directory;

}

const char *S9xGetFilename (const char *extension, enum s9x_getdirtype dirtype)
{

	// directory
	static char filename[PATH_MAX+1];
	char directory[PATH_MAX+1];

	// components
	char drive[10];
	char dir[PATH_MAX+1];
	char fname[PATH_MAX+1];
	char ext[PATH_MAX+1];

	strcpy(directory,S9xGetDirectory(dirtype));
	_splitpath(Memory.ROMFilename, drive, dir, fname, ext);

	snprintf(filename, PATH_MAX + 1, "%s%s%s%s", directory, SLASH_STR, fname, extension);

	return filename;

}

const char *S9xGetFilenameInc (const char *extension, enum s9x_getdirtype dirtype)
{

	static char filename[PATH_MAX+1];
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
		snprintf(filename, PATH_MAX + 1, "%s%s%s.%03d%s", directory, SLASH_STR, fname, i++, extension);

		if (i > 100)
		{
			break;
		}
	}
	while (!fileXioGetStat(filename, &stat));

	return filename;

}

const char *S9xChooseFilename (bool8 read_only)
{
	return NULL;
}

#ifdef MOVIE_SUPPORT
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
	printf("Saving SRAM\n");
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

static inline int __udelay(unsigned int usecs)
{
 
	register unsigned int loops_total = 0;
	register unsigned int loops_end   = usecs * 148;

	if (usecs > loops_end)
	{

		return -1;

	}

	asm volatile (".set noreorder\n\t"
				  "0:\n\t"
				  "beq %0,%2,0f\n\t"
				  "addiu %0,1\n\t"
				  "bne %0,%2,0b\n\t"
				  "addiu %0,1\n\t"
				  "0:\n\t"
				  ".set reorder\n\t"
				  :"=r" (loops_total)
				  :"0" (loops_total), "r" (loops_end));

	return 0;

}

void S9xSyncSpeed (void)
{

	if (Settings.SoundSync)
	{
		while (!S9xSyncSound());
			__udelay(0);
	}

	if (Settings.DumpStreams)
		return;

#ifdef NETPLAY_SUPPORT
	if (Settings.NetPlay && NetPlay.Connected)
	{
	#if defined(NP_DEBUG) && NP_DEBUG == 2
		printf("CLIENT: SyncSpeed @%d\n", S9xGetMilliTime());
	#endif

		S9xNPSendJoypadUpdate(old_joypads[0]);
		for (int J = 0; J < 8; J++)
			joypads[J] = S9xNPGetJoypad(J);

		if (!S9xNPCheckForHeartBeat())
		{
			NetPlay.PendingWait4Sync = !S9xNPWaitForHeartBeatDelay(100);
		#if defined(NP_DEBUG) && NP_DEBUG == 2
			if (NetPlay.PendingWait4Sync)
				printf("CLIENT: PendingWait4Sync1 @%d\n", S9xGetMilliTime());
		#endif

			IPPU.RenderThisFrame = TRUE;
			IPPU.SkippedFrames = 0;
		}
		else
		{
			NetPlay.PendingWait4Sync = !S9xNPWaitForHeartBeatDelay(200);
		#if defined(NP_DEBUG) && NP_DEBUG == 2
			if (NetPlay.PendingWait4Sync)
				printf("CLIENT: PendingWait4Sync2 @%d\n", S9xGetMilliTime());
		#endif

			if (IPPU.SkippedFrames < NetPlay.MaxFrameSkip)
			{
				IPPU.RenderThisFrame = FALSE;
				IPPU.SkippedFrames++;
			}
			else
			{
				IPPU.RenderThisFrame = TRUE;
				IPPU.SkippedFrames = 0;
			}
		}

		if (!NetPlay.PendingWait4Sync)
		{
			NetPlay.FrameCount++;
			S9xNPStepJoypadHistory();
		}

		return;
	}
#endif

	if (Settings.HighSpeedSeek > 0)
		Settings.HighSpeedSeek--;

	if (Settings.TurboMode)
	{
		if ((++IPPU.FrameSkip >= Settings.TurboSkipFrames) && !Settings.HighSpeedSeek)
		{
			IPPU.FrameSkip = 0;
			IPPU.SkippedFrames = 0;
			IPPU.RenderThisFrame = TRUE;
		}
		else
		{
			IPPU.SkippedFrames++;
			IPPU.RenderThisFrame = FALSE;
		}

		return;
	}

	static unsigned long next_ms = 0;
	static unsigned int next_sec = 0;

	unsigned long now_ms;
	unsigned int  now_sec;

	now_ms = (unsigned long)(((float)clock()) / 576.0f);
	now_sec = (unsigned int)(((float)now_ms) / 1000.0f);

	// If there is no known "next" frame, initialize it now.
	if (next_sec == 0)
	{
		next_sec = now_sec;
		next_ms = now_ms;
		next_ms++;
	}

	// If we're on AUTO_FRAMERATE, we'll display frames always only if there's excess time.
	// Otherwise we'll display the defined amount of frames.
	unsigned	limit = (Settings.SkipFrames == AUTO_FRAMERATE) ? (next_ms < now_ms ? 10 : 1) : Settings.SkipFrames;

	IPPU.RenderThisFrame = (++IPPU.SkippedFrames >= limit) ? TRUE : FALSE;

	if (IPPU.RenderThisFrame)
		IPPU.SkippedFrames = 0;
	else
	{
		// If we were behind the schedule, check how much it is.
		if (next_ms < now_ms)
		{
			unsigned lag = (now_sec - next_sec) * 1000 + now_ms - next_ms;

			if (lag >= 500)
			{
				// More than a half-second behind means probably pause.
				// The next line prevents the magic fast-forward effect.
				next_sec = now_sec;
				next_ms = now_ms;
			}
		}
	}

	// Delay until we're completed this frame.
	while (next_ms > now_ms)
	{
		unsigned timeleft = ((next_sec - now_sec) * 1000 + next_ms - now_ms)*1000;

		// If we're ahead of time, sleep a while.
		__udelay(timeleft);

		now_ms = (unsigned long)(((float)clock())/576.0f);
		now_sec = (unsigned int)(((float)now_ms) / 1000.0f);

	}

	// Calculate the timestamp of the next frame.
	next_ms += (unsigned long)(((float)Settings.FrameTime)/1000.0f);

	if (next_ms >= 1000)
	{
		next_sec += (unsigned int)(((float)next_ms) / 1000.0f);
		next_ms %= 1000;
	}

}

/*
unsigned long now_ms;
unsigned int  now_sec;

static unsigned long next_ms = 0;
static unsigned int next_sec = 0;

#undef TIMER_DIFF
#define TIMER_DIFF(a, b) (long)(((a##_sec - b##_sec) * 1000) + a##_ms - b##_ms)

void S9xSyncSpeedFinish (void)
{
	if (!syncing)
		return;

	now_ms = (unsigned long)(((float)clock()) / 576.0f);
	now_sec = (unsigned int)(((float)now_ms) / 1000.0f);

	if (Settings.SoundSync)
	{
		while (!S9xSyncSound ())
		{
			__udelay(100);

			next_ms = (unsigned long)(((float)clock()) / 576.0f);
			next_sec = (unsigned int)(((float)now_ms) / 1000.0f);

			// If we can't sync sound within a second, we're probably deadlocked
			if (TIMER_DIFF(next,now) > 1000)
			{
				// Flush out our sample buffer and give up.
				S9xClearSamples ();

				break;
			}
		}

		next_ms = now_ms;
		next_sec = now_sec;

		return;
	}

	if (TIMER_DIFF (next, now) < -500)
	{
		next_ms = now_ms;
		next_sec = now_sec;
	}

	while (next_ms > now_ms)
	{
		int time_left = TIMER_DIFF (next, now);

		if (time_left > 500)
		{
			next_ms = now_ms;
			next_sec = now_sec;
			break;
		}

		__udelay(time_left);

		now_ms = (unsigned long)(((float)clock()) / 576.0f);
		now_sec = (unsigned int)(((float)now_ms) / 1000.0f);
	}

	next_ms += (unsigned long)(((float)Settings.FrameTime)/1000.0f);

	if (next_ms >= 1000)
	{
		next_sec += (unsigned int)(((float)next_ms) / 1000.0f);
		next_ms %= 1000;
	}

	syncing = 0;

	return;
}

void S9xSyncSpeed (void)
{

	unsigned int limit;
	int          lag;
	int          frametime;



#ifdef NETPLAY_SUPPORT
	if (S9xNetplaySyncSpeed())
		return;
#endif


	if (Settings.HighSpeedSeek > 0)
	{
		Settings.HighSpeedSeek--;
		IPPU.RenderThisFrame = FALSE;
		IPPU.SkippedFrames = 0;

		now_ms = (unsigned long)(((float)clock()) / 576.0f);
		now_sec = (unsigned int)(((float)now_ms) / 1000.0f);

		next_ms = now_ms;
		next_sec = now_sec;

		syncing = 0;

		return;
	}

	if (Settings.TurboMode)
	{
		if ((++IPPU.FrameSkip >= Settings.TurboSkipFrames) && !Settings.HighSpeedSeek)
		{
			IPPU.FrameSkip = 0;
			IPPU.SkippedFrames = 0;
			IPPU.RenderThisFrame = TRUE;
		}
		else
		{
			IPPU.SkippedFrames++;
			IPPU.RenderThisFrame = FALSE;
		}

		return;
	}

	now_ms = (unsigned long)(((float)clock()) / 576.0f);
	now_sec = (unsigned int)(((float)now_ms) / 1000.0f);

	// If there is no known "next" frame, initialize it now.
	if (next_sec == 0)
	{
		next_sec = now_sec;
		next_ms = now_ms+1;
	}

	if (Settings.SkipFrames == AUTO_FRAMERATE && !Settings.SoundSync)
	{
		lag = TIMER_DIFF(now,next);
		frametime = (int)(((float)Settings.FrameTime)/1000.0f);

		if (lag > frametime)
		{
			if (lag > (frametime * 10))
			{
				next_ms = now_ms;
				next_sec = now_sec;
				IPPU.RenderThisFrame = 1;
				IPPU.SkippedFrames = 0;
			}
			else
			{
				IPPU.RenderThisFrame = 0;
				IPPU.SkippedFrames++;
			}
		}
		else
		{
			IPPU.RenderThisFrame = 1;
			IPPU.SkippedFrames = 0;
		}
	}
	else
	{
		limit = Settings.SoundSync ? 1 : Settings.SkipFrames + 1;

		IPPU.SkippedFrames++;
		IPPU.RenderThisFrame = 0;

		if (IPPU.SkippedFrames >= limit)
		{
			IPPU.RenderThisFrame = 1;
			IPPU.SkippedFrames = 0;
		}
	}

	syncing = 1;

	return;

}
*/

