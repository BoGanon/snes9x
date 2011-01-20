#include <iostream>
#include <string.h>

#include <port.h>
#include <crosshairs.h>
#include <controls.h>
#include <snes9x.h>

#include <cfg.h>
#include <libconfig.h>

#include <snes9x_cfg.h>

int buffer_ms = 96;
int lag_ms = 0;

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		char port[2][25];
		char xhair[5][25];
	} control_settings_t;

	static control_settings_t control_settings;

#ifdef DEBUGGER
	static bool8 debug_settings[2];
#endif

#ifdef __cplusplus
};
#endif

static bool parse_controller_spec (int port, const char *arg)
{
	if (!strcasecmp(arg, "none"))
		S9xSetController(port, CTL_NONE,       0, 0, 0, 0);
	else
	if (!strncasecmp(arg, "pad",   3) && arg[3] >= '1' && arg[3] <= '8' && arg[4] == '\0')
		S9xSetController(port, CTL_JOYPAD, arg[3] - '1', 0, 0, 0);
	else
	if (!strncasecmp(arg, "mouse", 5) && arg[5] >= '1' && arg[5] <= '2' && arg[6] == '\0')
		S9xSetController(port, CTL_MOUSE,  arg[5] - '1', 0, 0, 0);
	else
	if (!strcasecmp(arg, "superscope"))
		S9xSetController(1, CTL_SUPERSCOPE, 0, 0, 0, 0);
	else
	if (!strcasecmp(arg, "justifier"))
		S9xSetController(1, CTL_JUSTIFIER,  0, 0, 0, 0);
	else
	if (!strcasecmp(arg, "two-justifiers"))
		S9xSetController(1, CTL_JUSTIFIER,  1, 0, 0, 0);
	else
	if (!strncasecmp(arg, "mp5:", 4) && ((arg[4] >= '1' && arg[4] <= '8') || arg[4] == 'n') &&
										((arg[5] >= '1' && arg[5] <= '8') || arg[5] == 'n') &&
										((arg[6] >= '1' && arg[6] <= '8') || arg[6] == 'n') &&
										((arg[7] >= '1' && arg[7] <= '8') || arg[7] == 'n') && arg[8] == '\0')
		S9xSetController(port, CTL_MP5, (arg[4] == 'n') ? -1 : arg[4] - '1',
										(arg[5] == 'n') ? -1 : arg[5] - '1',
										(arg[6] == 'n') ? -1 : arg[6] - '1',
										(arg[7] == 'n') ? -1 : arg[7] - '1');
	else
		return (false);

	return (true);
}

static void parse_crosshair_spec (enum crosscontrols ctl, const char *spec)
{
	int			idx = -1, i;
	const char	*fg = NULL, *bg = NULL, *s = spec;

	if (s[0] == '"')
	{
		s++;
		for (i = 0; s[i] != '\0'; i++)
			if (s[i] == '"' && s[i - 1] != '\\')
				break;

		idx = 31 - ctl;

		std::string	fname(s, i);
		if (!S9xLoadCrosshairFile(idx, fname.c_str()))
			return;

		s += i + 1;
	}
	else
	{
		if (isdigit(*s))
		{
			idx = *s - '0';
			s++;
		}

		if (isdigit(*s))
		{
			idx = idx * 10 + *s - '0';
			s++;
		}

		if (idx > 31)
		{
#ifdef DEBUG
			fprintf(stderr, "Invalid crosshair spec '%s'.\n", spec);
#endif
			return;
		}
	}

	while (*s != '\0' && isspace(*s))
		s++;

	if (*s != '\0')
	{
		fg = s;

		while (isalnum(*s))
			s++;

		if (*s != '/' || !isalnum(s[1]))
		{
#ifdef DEBUG
			fprintf(stderr, "Invalid crosshair spec '%s.'\n", spec);
#endif
			return;
		}

		bg = ++s;

		while (isalnum(*s))
			s++;

		if (*s != '\0')
		{
#ifdef DEBUG
			fprintf(stderr, "Invalid crosshair spec '%s'.\n", spec);
#endif
			return;
		}
	}

	S9xSetControllerCrosshair(ctl, idx, fg, bg);
}


void S9xInitSettings(void)
{

	ZeroMemory(&Settings,sizeof(Settings));

	Settings.MouseMaster = TRUE;
	Settings.SuperScopeMaster = TRUE;
	Settings.JustifierMaster = TRUE;
	Settings.MultiPlayer5Master = TRUE;
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.SixteenBitSound = TRUE;
	Settings.Stereo = TRUE;
	Settings.SoundPlaybackRate = 48000;
	Settings.SoundInputRate = 16000;
	Settings.SupportHiRes = TRUE;
	Settings.Transparency = TRUE;
	Settings.AutoDisplayMessages = TRUE;
	Settings.InitialInfoStringTimeout = 120;
	Settings.HDMATimingHack = 100;
	Settings.BlockInvalidVRAMAccessMaster = TRUE;
	Settings.StopEmulation = TRUE;
	Settings.WrongMovieStateProtection = TRUE;
	Settings.DumpStreamsMaxFrames = -1;
	Settings.StretchScreenshots = 1;
	Settings.SnapshotScreenshots = TRUE;
	Settings.SkipFrames = AUTO_FRAMERATE;
	Settings.TurboSkipFrames = 15;
	Settings.CartAName[0] = 0;
	Settings.CartBName[0] = 0;

#ifdef NETPLAY_SUPPORT
	Settings.ServerName[0] = 0;
#endif

}

void S9xParseCFG(config_t *config)
{

	bool8 autoframeskip = FALSE;
	char section_path[256];
	char setting[256];

	/// ROM
	sprintf(section_path,"%s%s",SNES9X_SECTION,".ROM.");

	sprintf(setting,"%s%s",section_path,"Interleaved2");
	Settings.ForceInterleaved2 =           cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"InterleaveGD24");
	Settings.ForceInterleaveGD24 =         cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"Cheat");
	Settings.ApplyCheats =                 cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"Patch");
	Settings.NoPatch =                    !cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"LoROM");
	Settings.ForceLoROM =                  cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"HiROM");
	Settings.ForceHiROM =                  cfg_get_bool(setting, FALSE);

	if (Settings.ForceLoROM)
	{
		Settings.ForceHiROM = FALSE;
	}

	sprintf(setting,"%s%s",section_path,"PAL");
	Settings.ForcePAL   =                  cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"NTSC");
	Settings.ForceNTSC  =                  cfg_get_bool(setting, FALSE);

	if (Settings.ForcePAL)
	{
		Settings.ForceNTSC = FALSE;
	}

	sprintf(setting,"%s%s",section_path,"Header");
	Settings.ForceHeader =                 cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"NoHeader");
	Settings.ForceNoHeader =               cfg_get_bool(setting, FALSE);

	if (Settings.ForceHeader)
	{
		Settings.ForceNoHeader = !Settings.ForceHeader;
	}

	sprintf(setting,"%s%s",section_path,"Interleaved");
	Settings.ForceInterleaved =            cfg_get_bool(setting, FALSE);

	if (Settings.ForceInterleaved)
	{
		Settings.ForceNotInterleaved = !Settings.ForceInterleaved;
	}

	/// Sound
	sprintf(section_path,"%s%s",SNES9X_SECTION,".Sound.");

	sprintf(setting,"%s%s",section_path,"BufferMS");
	buffer_ms =                  (unsigned int)cfg_string_to_int(cfg_get_string(setting, "96"));

	sprintf(setting,"%s%s",section_path,"LagMS");
	lag_ms =                     (unsigned int)cfg_string_to_int(cfg_get_string(setting, "0"));

	sprintf(setting,"%s%s",section_path,"Sync");
	Settings.SoundSync =                   cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"SixteenBitSound");
	Settings.SixteenBitSound =             cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"Stereo");
	Settings.Stereo =                      cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"ReverseStereo");
	Settings.ReverseStereo =               cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"Rate");
	Settings.SoundPlaybackRate = (unsigned int)cfg_string_to_int(cfg_get_string(setting, "48000"));

	sprintf(setting,"%s%s",section_path,"InputRate");
	Settings.SoundInputRate =    (unsigned int)cfg_string_to_int(cfg_get_string(setting, "22000"));

	sprintf(setting,"%s%s",section_path,"Mute");
	Settings.Mute =                        cfg_get_bool(setting, FALSE);

	/// Display
	sprintf(section_path,"%s%s",SNES9X_SECTION,".Display.");

	sprintf(setting,"%s%s",section_path,"HiRes");
	Settings.SupportHiRes =                cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"Transparency");
	Settings.Transparency =                cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"GraphicWindows");
	Settings.DisableGraphicWindows  =     !cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"DisplayFrameRate");
	Settings.DisplayFrameRate =            cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"DisplayWatchedAddresses");
	Settings.DisplayWatchedAddresses =     cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"DisplayInput");
	Settings.DisplayPressedKeys =          cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"DisplayFrameCount");
	Settings.DisplayMovieFrame =           cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"MessagesInImage");
	Settings.AutoDisplayMessages =         cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"MessageDisplayTime");
	Settings.InitialInfoStringTimeout =        cfg_string_to_int(cfg_get_string(setting,  "120"));

	/// Settings
	sprintf(section_path,"%s%s",SNES9X_SECTION,".Settings.");

	sprintf(setting,"%s%s",section_path,"BSXBootup");
	Settings.BSXBootup =                   cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"TurboMode");
	Settings.TurboMode =                   cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"TurboFrameSkip");
	Settings.TurboSkipFrames =   (unsigned int)cfg_string_to_int(cfg_get_string(setting, "15"));

	sprintf(setting,"%s%s",section_path,"MovieTruncateAtEnd");
	Settings.MovieTruncate =               cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"MovieNotifyIgnored");
	Settings.MovieNotifyIgnored =          cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"WrongMovieStateProtection");
	Settings.WrongMovieStateProtection =   cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"StretchScreenshots");
	Settings.StretchScreenshots =              cfg_string_to_int(cfg_get_string(setting,  "1"));

	sprintf(setting,"%s%s",section_path,"SnapshotScreenshots");
	Settings.SnapshotScreenshots =         cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"DontSaveOopsSnapshot");
	Settings.DontSaveOopsSnapshot =        cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"AutoSaveDelay");
	Settings.AutoSaveDelay =     (unsigned int)cfg_string_to_int(cfg_get_string(setting, "0"));

	sprintf(setting,"%s%s",section_path,"FrameTimePAL");
	Settings.FrameTimePAL =      (unsigned int)cfg_string_to_int(cfg_get_string(setting, "20000"));

	sprintf(setting,"%s%s",section_path,"FrameTimeNTSC");
	Settings.FrameTimeNTSC =     (unsigned int)cfg_string_to_int(cfg_get_string(setting, "16667"));

	sprintf(setting,"%s%s",section_path,"AutoFrameSkip");
	autoframeskip =                        cfg_get_bool(setting, FALSE);

	if (autoframeskip)
	{
		Settings.SkipFrames = AUTO_FRAMERATE;
	}
	else
	{
		sprintf(setting,"%s%s",section_path,"FrameSkip");
		Settings.SkipFrames =    (unsigned int)cfg_string_to_int(cfg_get_string(setting, "1")) + 1;
	}

	/// Controls
	sprintf(section_path,"%s%s",SNES9X_SECTION,".Controls.");

	sprintf(setting,"%s%s",section_path,"MouseMaster");
	Settings.MouseMaster =                 cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"SuperscopeMaster");
	Settings.SuperScopeMaster =            cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"JustifierMaster");
	Settings.JustifierMaster =             cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"MP5Master");
	Settings.MultiPlayer5Master =          cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"AllowLeftRight");
	Settings.UpAndDown =                   cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"Port1");
	strcpy(control_settings.port[0],       cfg_get_string(setting,"pad1"));
	parse_controller_spec(0,control_settings.port[0]);

	sprintf(setting,"%s%s",section_path,"Port2");
	strcpy(control_settings.port[1],       cfg_get_string(setting,"pad2"));
	parse_controller_spec(1,control_settings.port[1]);

	sprintf(setting,"%s%s",section_path,"Mouse1Crosshair");
	strcpy(control_settings.xhair[0],      cfg_get_string(setting,"1 White/Black"));
	parse_crosshair_spec(X_MOUSE1,control_settings.xhair[0]);

	sprintf(setting,"%s%s",section_path,"Mouse2Crosshair");
	strcpy(control_settings.xhair[1],      cfg_get_string(setting,"1 White/Black"));
	parse_crosshair_spec(X_MOUSE2,control_settings.xhair[1]);

	sprintf(setting,"%s%s",section_path,"SuperscopeCrosshair");
	strcpy(control_settings.xhair[2],      cfg_get_string(setting,"2 White/Black"));
	parse_crosshair_spec(X_SUPERSCOPE,control_settings.xhair[2]);

	sprintf(setting,"%s%s",section_path,"Justifier1Crosshair");
	strcpy(control_settings.xhair[3],      cfg_get_string(setting,"4 Blue/Black"));
	parse_crosshair_spec(X_JUSTIFIER1,control_settings.xhair[3]);

	sprintf(setting,"%s%s",section_path,"Justifier2Crosshair");
	strcpy(control_settings.xhair[4],      cfg_get_string(setting,"4 MagicPink/Black"));
	parse_crosshair_spec(X_JUSTIFIER2,control_settings.xhair[4]);

	/// Hacks
	sprintf(section_path,"%s%s",SNES9X_SECTION,".Hacks.");

	sprintf(setting,"%s%s",section_path,"EnableGameSpecificHacks");
	Settings.DisableGameSpecificHacks =   !cfg_get_bool(setting, TRUE);

	sprintf(setting,"%s%s",section_path,"AllowInvalidVRAMAccess");
	Settings.BlockInvalidVRAMAccess =     !cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"SpeedHacks");
	Settings.ShutdownMaster =              cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"DisableIRQ");
	Settings.DisableIRQ  =                 cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"DisableHDMA");
	Settings.DisableHDMA =                 cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"HDMATiming");
	Settings.HDMATimingHack =              cfg_string_to_int(cfg_get_string(setting, "100"));

#ifdef NETPLAY_SUPPORT
	/// Netplay
	sprintf(section_path,"%s%s",SNES9X_SECTION,".Netplay.");

	sprintf(setting,"%s%s",section_path,"Enable");
	Settings.NetPlay =                     cfg_get_bool(setting, FALSE);

	sprintf(setting,"%s%s",section_path,"Port");
	Settings.Port =                       -cfg_string_to_int(cfg_get_string(setting,NP_DEFAULT_PORT));

	sprintf(setting,"%s%s",section_path,"Server");
	Settings.ServerName[0] = '\0';
	if (cfg_lookup(setting) != NULL)
		strncpy(Settings.ServerName,       cfg_get_string(setting, "Snes9x Server"), 128);
#endif

#ifdef DEBUGGER
	/// Debug
	sprintf(section_path,"%s%s",SNES9X_SECTION,".DEBUG.");

	sprintf(setting,"%s%s",section_path,"Debugger");
	debug_settings[0] =                    cfg_get_bool(setting, FALSE);

	if (debug_settings[0])
		CPU.Flags |= DEBUG_MODE_FLAG;

	sprintf(setting,"%s%s",section_path,"Trace");
	debug_settings[1] =                    cfg_get_bool(setting, FALSE);

	if (debug_settings[1])
	{
		if (!trace)
			trace = fopen("trace.log", "wb");
		CPU.Flags |= TRACE_FLAG;
	}
#endif

	S9xVerifyControllers();

}

void S9xAddSettingsToCFG(config_t *config)
{

	char value[256];
	config_setting_t *root;
	config_setting_t *group;
	config_setting_t *subgroup;
	config_setting_t *setting;

	root = config_root_setting(config);

	group = config_setting_add(root,SNES9X_SECTION, CONFIG_TYPE_GROUP);
	{

		subgroup = config_setting_add(group,"ROM",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(subgroup,"Interleaved2",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceInterleaved2);

			setting = config_setting_add(subgroup,"InterleaveGD24",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceInterleaveGD24);

			setting = config_setting_add(subgroup,"Cheat",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ApplyCheats);

			setting = config_setting_add(subgroup,"Patch",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,!Settings.NoPatch);

			setting = config_setting_add(subgroup,"LoROM",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceLoROM);

			setting = config_setting_add(subgroup,"HiROM",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceHiROM);

			setting = config_setting_add(subgroup,"PAL",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForcePAL);

			setting = config_setting_add(subgroup,"NTSC",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceNTSC);

			setting = config_setting_add(subgroup,"Header",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceHeader);

			setting = config_setting_add(subgroup,"NoHeader",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceNoHeader);

			setting = config_setting_add(subgroup,"Interleaved",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ForceInterleaved);

		}

		subgroup = config_setting_add(group,"Sound",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(subgroup,"BufferMS",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,buffer_ms);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"LagMS",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,lag_ms);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"Sync",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.SoundSync);

			setting = config_setting_add(subgroup,"SixteenBitSound",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.SixteenBitSound);

			setting = config_setting_add(subgroup,"Stereo",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.Stereo);

			setting = config_setting_add(subgroup,"ReverseStereo",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ReverseStereo);

			setting = config_setting_add(subgroup,"Rate",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,(int)Settings.SoundPlaybackRate);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"InputRate",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,(int)Settings.SoundInputRate);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"Mute",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.Mute);

		}

		subgroup = config_setting_add(group,"Display",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(subgroup,"HiRes",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.SupportHiRes);

			setting = config_setting_add(subgroup,"Transparency",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.Transparency);

			setting = config_setting_add(subgroup,"GraphicWindows",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,!Settings.DisableGraphicWindows);

			setting = config_setting_add(subgroup,"DisplayFrameRate",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.DisplayFrameRate);

			setting = config_setting_add(subgroup,"DisplayWatchedAddresses",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.DisplayWatchedAddresses);

			setting = config_setting_add(subgroup,"DisplayInput",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.DisplayPressedKeys);

			setting = config_setting_add(subgroup,"DisplayFrameCount",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.DisplayMovieFrame);

			setting = config_setting_add(subgroup,"MessagesInImage",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.AutoDisplayMessages);

			setting = config_setting_add(subgroup,"MessageDisplayTime",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,Settings.InitialInfoStringTimeout);
			config_setting_set_string(setting,value);

		}

		subgroup = config_setting_add(group,"Settings",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(subgroup,"BSXBootup",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.BSXBootup);

			setting = config_setting_add(subgroup,"TurboMode",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.TurboMode);

			setting = config_setting_add(subgroup,"TurboFrameSkip",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,(int)Settings.TurboSkipFrames);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"MovieTruncateAtEnd",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.MovieTruncate);

			setting = config_setting_add(subgroup,"MovieNotifyIgnored",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.MovieNotifyIgnored);

			setting = config_setting_add(subgroup,"WrongMovieStateProtection",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.WrongMovieStateProtection);

			setting = config_setting_add(subgroup,"StretchScreenshots",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,Settings.StretchScreenshots);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"SnapshotScreenshots",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.SnapshotScreenshots);

			setting = config_setting_add(subgroup,"DontSaveOopsSnapshot",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.DontSaveOopsSnapshot);

			setting = config_setting_add(subgroup,"AutoSaveDelay",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,(int)Settings.AutoSaveDelay);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"FrameTimePAL",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,(int)Settings.FrameTimePAL);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"FrameTimeNTSC",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,(int)Settings.FrameTimeNTSC);
			config_setting_set_string(setting,value);

			
			if (Settings.SkipFrames == AUTO_FRAMERATE)
			{
				setting = config_setting_add(subgroup,"AutoFrameSkip",CONFIG_TYPE_BOOL);
				config_setting_set_bool(setting,1);
			}
			else
			{
				setting = config_setting_add(subgroup,"AutoFrameSkip",CONFIG_TYPE_BOOL);
				config_setting_set_bool(setting,0);

				setting = config_setting_add(subgroup,"FrameSkip",CONFIG_TYPE_STRING);
				cfg_int_to_string(value,(int)Settings.SkipFrames-1);
				config_setting_set_string(setting,value);
			}
		}

		subgroup = config_setting_add(group,"Controls",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(subgroup,"MouseMaster",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.MouseMaster);

			setting = config_setting_add(subgroup,"SuperscopeMaster",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.SuperScopeMaster);

			setting = config_setting_add(subgroup,"JustifierMaster",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.JustifierMaster);

			setting = config_setting_add(subgroup,"MP5Master",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.MultiPlayer5Master);

			setting = config_setting_add(subgroup,"AllowLeftRight",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.UpAndDown);

			setting = config_setting_add(subgroup,"Port1",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,control_settings.port[0]);

			setting = config_setting_add(subgroup,"Port2",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,control_settings.port[1]);

			setting = config_setting_add(subgroup,"Mouse1Crosshair",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,control_settings.xhair[0]);

			setting = config_setting_add(subgroup,"Mouse2Crosshair",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,control_settings.xhair[1]);

			setting = config_setting_add(subgroup,"SuperscopeCrosshair",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,control_settings.xhair[2]);

			setting = config_setting_add(subgroup,"Justifier1Crosshair",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,control_settings.xhair[3]);

			setting = config_setting_add(subgroup,"Justifier2Crosshair",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,control_settings.xhair[4]);

		}

		subgroup = config_setting_add(group,"Hacks",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(subgroup,"EnableGameSpecificHacks",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,!Settings.DisableGameSpecificHacks);

			setting = config_setting_add(subgroup,"AllowInvalidVRAMAccess",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,!Settings.BlockInvalidVRAMAccess);

			setting = config_setting_add(subgroup,"SpeedHacks",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.ShutdownMaster);

			setting = config_setting_add(subgroup,"DisableIRQ",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.DisableIRQ);

			setting = config_setting_add(subgroup,"DisableHDMA",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.DisableHDMA);

			setting = config_setting_add(subgroup,"HDMATiming",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,Settings.HDMATimingHack);
			config_setting_set_string(setting,value);

		}

#ifdef NETPLAY_SUPPORT
		subgroup = config_setting_add(group,"Netplay",CONFIG_TYPE_GROUP);
		{
			setting = config_setting_add(subgroup,"Enable",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,Settings.NetPlay);

			setting = config_setting_add(subgroup,"Port",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,Settings.Port);
			config_setting_set_string(setting,value);

			setting = config_setting_add(subgroup,"Server",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,Settings.ServerName);
		}
#endif

#ifdef DEBUGGER
		subgroup = config_setting_add(group,"DEBUG",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(subgroup,"Debugger");
			config_setting_set_bool(setting,debug_settings[0]);

			setting = config_setting_add(subgroup,"Trace");
			config_setting_set_bool(setting,debug_settings[1]);

		}
#endif

	}

}

void S9xLoadCFG(char *path)
{

	config_t *cfg = cfg_open(path);

	S9xParseCFG(cfg);

	cfg_close(cfg);

}

void S9xSaveCFG(char *path)
{

	FILE *file;

	config_t *cfg = cfg_open(NULL);

	S9xAddSettingsToCFG(cfg);

	file = fopen(path,"w+");

	if (file == NULL)
	{
		return;
	}

	config_write(cfg,file);

	fclose(file);

	cfg_close(cfg);

}
