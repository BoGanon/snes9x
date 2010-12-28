#include <libmtap.h>
#include <input.h>

#include <libconfig.h>
#include <cfg.h>

#include <ps2_controls.h>
#include <controls.h>


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

	char names[20][17] = 
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
		"Up/Down T=50%",
		"Left/Right T=50%",
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
	int base = k_PS2_DS_BASE | (n << 16);

	// 16 buttons
	for (i = 0; i < 16; i++)
	{
		S9xMapButton(base + i, S9xGetCommandT(mapping[i]), false);
		//printf("Mapping = %s\n",S9xGetCommandName(S9xGetMapping(base+i)));
	}

	// 4 axes
	for ( ; i < 20; i++)
	{
		S9xMapAxis(base + i, S9xGetCommandT(mapping[i]), false);
		//printf("Axis Mapping = %s\n",S9xGetCommandName(S9xGetMapping(base+i)));

	}

}

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

void ps2_input_init(config_t *config)
{

	int i,j;

	char **mapping = (char**)malloc(sizeof(char*)*20);

	char pad_path[256];
	char setting[256];

	char **default_mapping = ps2_input_mapping_init();

	S9xUnmapAllControls();

	ps2_input_map_pseudo();

	for (i = 1; i < 9; i++)
	{
		sprintf(pad_path,"%s.%s%d","PS2","Pad",i);

		ps2_input_map_default(i,default_mapping);

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

void ps2_input_open_pads()
{

	int i;

	pad_t *pad;

	// hardcoded controllers
	for (i = 0; i < 1; i++)
	{

		pad = pad_open(multitap_ports[i],multitap_slots[i],MODE_ANALOG,MODE_UNLOCKED);

		if (pad != NULL)
		{
			printf("(port,slot) = (%d,%d)\n",multitap_ports[i],multitap_slots[i]);

			pads[pad_num] = pad;
			pad_num++;
		}
	}

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

	int n;

	int new_data;
	int old_data = 0;

	int base;

	for (n = 0; n < pad_num; n++)
	{
		base = k_PS2_DS_BASE | ((n+1) << 16);

		pad_get_buttons(pads[n]);
		pads[n]->buttons->btns ^= 0xFFFF;

		new_data = pads[n]->buttons->btns & old_data;
		old_data = ~pads[n]->buttons->btns;

		S9xReportButton(base +  k_SELECT, (new_data & PAD_SELECT));
		S9xReportButton(base +  k_L3, (new_data & PAD_L3));
		S9xReportButton(base +  k_R3, (new_data & PAD_R3));
		S9xReportButton(base +  k_START, (new_data & PAD_START));
		S9xReportButton(base +  k_UP, (new_data & PAD_UP));
		S9xReportButton(base +  k_RIGHT, (new_data & PAD_RIGHT));
		S9xReportButton(base +  k_DOWN, (new_data & PAD_DOWN));
		S9xReportButton(base +  k_LEFT, (new_data & PAD_LEFT));
		S9xReportButton(base +  k_L2, (new_data & PAD_L2));
		S9xReportButton(base +  k_R2, (new_data & PAD_R2));
		S9xReportButton(base +  k_L1, (new_data & PAD_L1));
		S9xReportButton(base +  k_R1, (new_data & PAD_R1));
		S9xReportButton(base +  k_TRIANGLE, (new_data & PAD_TRIANGLE));
		S9xReportButton(base +  k_CIRCLE, (new_data & PAD_CIRCLE));
		S9xReportButton(base +  k_CROSS, (new_data & PAD_CROSS));
		S9xReportButton(base +  k_SQUARE, (new_data & PAD_SQUARE));
		S9xReportAxis(  base +  k_L_V, ((pads[n]->buttons->ljoy_v-128)*255));
		S9xReportAxis(  base +  k_L_H, ((pads[n]->buttons->ljoy_h-128)*255));
		S9xReportAxis(  base +  k_R_V, ((pads[n]->buttons->rjoy_v-128)*255));
		S9xReportAxis(  base +  k_R_H, ((pads[n]->buttons->rjoy_h-128)*255));
	}
}
