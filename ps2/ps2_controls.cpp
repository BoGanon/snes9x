#include <libmtap.h>
#include <input.h>

#include <libconfig.h>
#include <cfg.h>

#include <ps2_controls.h>
#include <controls.h>
#include <snes9x_cfg.h>

#include <snes9x.h>

static pad_t *pads[8];
static int pad_num = 0;

char multitap_ports[8] =
{
	0, 0, 0, 0,
	1, 1, 1, 1
};

char multitap_slots[8] = 
{
	0, 1, 2, 3,
	0, 1, 2, 3
};

static const char ps2_buttons[20][9] =
{
	"Select",
	"L3",
	"R3",
	"Start",
	"Up",
	"Right",
	"Down",
	"Left",
	"L2",
	"R2",
	"L1",
	"R1",
	"Triangle",
	"Circle",
	"Cross",
	"Square",
	"L_V",
	"L_H",
	"R_V",
	"R_H"
};

void ps2_input_open_pads()
{

	int i = 0;

	pad_t *pad;

	// hardcoded controllers
	for (i = 0; i < 8; i++)
	{

		pad = pad_open(multitap_ports[i],multitap_slots[i],MODE_DIGITAL,MODE_UNLOCKED);

		if (pad != NULL)
		{
			printf("Pad(%d,%d) initialized.\n",multitap_ports[i],multitap_slots[i]);

			pads[pad_num] = pad;
			pad_num++;
		}
	}

}

char **ps2_input_mapping_init()
{

	int i;
	char **mapping = (char**)malloc(sizeof(char*)*20);

	for (i = 0; i < 20; i++)
	{
		mapping[i] = (char*)malloc(50);
	}

	return mapping;

}

void ps2_input_mapping_free(char **mapping)
{
	int i;

	for (i = 0; i < 20; i++)
	{
		free(mapping[i]);
	}

	free(mapping);
}


void ps2_input_map_default(int n, char **mapping)
{

	int i;

	char names[20][20] = 
	{
		"Select",
		"None",
		"None",
		"Start",
		"Up",
		"Right",
		"Down",
		"Left",
		"None",
		"None",
		"L",
		"R",
		"X",
		"A",
		"B",
		"Y",
		"None",
		"None",
		"None",
		"None"
	};


	for (i = 0; i < 16; i++)
	{
		sprintf(mapping[i],"Joypad%d %s", n, names[i]);
	}

	for (; i < 20; i++)
	{
		sprintf(mapping[i],"Joypad%d Axis %s", n, names[i]);
	}

}

void ps2_input_map_pseudo(void)
{

	S9xMapPointer(PseudoPointerBase + k_Pseudo_Mouse1, S9xGetCommandT("Pointer Mouse1"), false);
	S9xMapPointer(PseudoPointerBase + k_Pseudo_Mouse2, S9xGetCommandT("Pointer Mouse2"), false);
	S9xMapPointer(PseudoPointerBase + k_Pseudo_SuperScope, S9xGetCommandT("Pointer Superscope"), false);
	S9xMapPointer(PseudoPointerBase + k_Pseudo_Justifier1, S9xGetCommandT("Pointer Justifier1"), false);
	S9xMapPointer(PseudoPointerBase + k_Pseudo_Justifier2, S9xGetCommandT("Pointer Justifier2"), false);

}

void ps2_input_map_real(unsigned int n, char **mapping)
{

	int i;
	int base = 0;
	int button = 0;

	base = k_PS2_DS_BASE | ((n) << 16);

	// 16 buttons
	for (i = 0; i < 16; i++)
	{
		button = base + i;
		S9xMapButton(button, S9xGetCommandT(mapping[i]), false);
		//printf("%08x = %s\n",button,S9xGetCommandName(S9xGetMapping(button)));
	}

	// 4 axes
	for ( ; i < 20; i++)
	{
		button = base + i;
		S9xMapAxis(button, S9xGetCommandT(mapping[i]), false);
		//printf("%08x = %s\n",button,S9xGetCommandName(S9xGetMapping(button)));
	}

}

void ps2_input_init(config_t *config)
{

	int i,j;

	char **mapping;

	char pad_path[256];
	char setting[256];

	char **default_mapping = ps2_input_mapping_init();

	S9xUnmapAllControls();

	ps2_input_map_pseudo();

	mapping = (char**)malloc(sizeof(char*)*20);

	for (i = 0; i < 8; i++)
	{

		// Goes through Pad0 through Pad7
		sprintf(pad_path,"%s.%s%d",SNES9X_SECTION,"Pad",i);

		// Needs to start with Joypad1
		ps2_input_map_default(i+1,default_mapping);

		for (j = 0; j < 20; j++)
		{
			sprintf(setting,"%s.%s",pad_path,ps2_buttons[j]);

			mapping[j] = strdup(cfg_get_string(setting,default_mapping[j]));
		}

		ps2_input_map_real(i,mapping);

	}

	ps2_input_mapping_free(default_mapping);
	ps2_input_mapping_free(mapping);

}

void ps2_input_cfg(char *path)
{

	config_t *cfg = cfg_open(path);

	ps2_input_init(cfg);

	cfg_close(cfg);

}


void ps2_input_close_pads()
{
	int i;

	for (i = 0; i < pad_num; i++)
	{
		pad_close(pads[i]);
	}

	pad_num = 0;

}

void ps2_input_report_pads()
{

	static int stop = 0;

	static int first = 0;

	int n;
	int base;

	for (n = 0; n < pad_num; n++)
	{

		// Check if disconnected
		if (!pads[n]->state)
		{

			first = 0;

			if (n == 0)
			{
				first = 1;
			}

			continue;
		}

		pad_get_buttons(pads[n]);
		pads[n]->buttons->btns ^= 0xFFFF;

		if (pads[first]->buttons->btns != PAD_START)
		{
			stop = 0;
		}
		else
		{

			stop++;

			if (stop == 100)
			{
				S9xExit();
			}

		}

		base = k_PS2_DS_BASE | ((n) << 16);

		S9xReportButton(base +  k_SELECT, (pads[n]->buttons->btns & PAD_SELECT));
		S9xReportButton(base +  k_L3, (pads[n]->buttons->btns & PAD_L3));
		S9xReportButton(base +  k_R3, (pads[n]->buttons->btns & PAD_R3));
		S9xReportButton(base +  k_START, (pads[n]->buttons->btns & PAD_START));
		S9xReportButton(base +  k_UP, (pads[n]->buttons->btns & PAD_UP));
		S9xReportButton(base +  k_RIGHT, (pads[n]->buttons->btns & PAD_RIGHT));
		S9xReportButton(base +  k_DOWN, (pads[n]->buttons->btns & PAD_DOWN));
		S9xReportButton(base +  k_LEFT, (pads[n]->buttons->btns & PAD_LEFT));
		S9xReportButton(base +  k_L2, (pads[n]->buttons->btns & PAD_L2));
		S9xReportButton(base +  k_R2, (pads[n]->buttons->btns & PAD_R2));
		S9xReportButton(base +  k_L1, (pads[n]->buttons->btns & PAD_L1));
		S9xReportButton(base +  k_R1, (pads[n]->buttons->btns & PAD_R1));
		S9xReportButton(base +  k_TRIANGLE, (pads[n]->buttons->btns & PAD_TRIANGLE));
		S9xReportButton(base +  k_CIRCLE, (pads[n]->buttons->btns & PAD_CIRCLE));
		S9xReportButton(base +  k_CROSS, (pads[n]->buttons->btns & PAD_CROSS));
		S9xReportButton(base +  k_SQUARE, (pads[n]->buttons->btns & PAD_SQUARE));

		S9xReportAxis(  base +  k_L_V, ((pads[n]->buttons->ljoy_v-128)*255));
		S9xReportAxis(  base +  k_L_H, ((pads[n]->buttons->ljoy_h-128)*255));
		S9xReportAxis(  base +  k_R_V, ((pads[n]->buttons->rjoy_v-128)*255));
		S9xReportAxis(  base +  k_R_H, ((pads[n]->buttons->rjoy_h-128)*255));

	}

}
