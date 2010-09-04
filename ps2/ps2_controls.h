#ifndef __PS2_CONTROLS_H__
#define __PS2_CONTROLS_H__

// snes_pad
enum
{
	k_X = 0,
	k_A,
	k_B,
	k_Y,
	k_L,
	k_R,
	k_Select,
	k_Start,
	k_Up,
	k_Down,
	k_Left,
	k_Right,
	k_Axis
};

// snes_mouse
enum
{
	k_ML = 0,
	k_MR
};

// snes_scope
enum
{
	k_ScopeOffscreen = 0,
	k_ScopeFire,
	k_ScopeCursor,
	k_ScopeTurbo,
	k_ScopePause
};

// snes_lightgun
enum
{
	k_JustifierOffscreen = 0,
	k_JustifierTrigger,
	k_JustifierStart
};

enum
{
	k_SnesMouseNum     = 2,
	k_SnesJustifierNum = 2,
	k_SnesPadNum       = 8
};
	


/*
	JoypadN
	MouseN
	SuperScope
	JustifierN
	Pointer MouseN
	Pointer SuperScope
	Pointer JustifierN
*/

char snes_devices[5][12] =
{
	"Joypad",
	"Mouse",
	"SuperScope",
	"Justifier",
	"Pointer"
};

char snes_joypad_modifiers[3][7] =
{
	"Toggle",
	"Sticky",
	"Turbo"
	// StickyTurbo
	// ToggleSticky
	// ToggleTurbo
	// ToggleStickyTurbo
};

char snes_joypad_buttons[12][7] =
{
	"X",
	"A",
	"B",
	"Y",
	"L",
	"R",
	"Select",
	"Start",
	"Up",
	"Down",
	"Left",
	"Right",
	"Axis"
};

char snes_joypad_axes[10][11] =
{
	"Left/Right",
	"Right/Left",
	"Up/Down",
	"Down/Up",
	"Y/A",
	"A/Y",
	"X/B",
	"B/X",
	"L/R",
	"R/L"
};

char snes_mouse_buttons[3][3] =
{
	"L",
	"R",
	"LR"
};

char snes_scope_buttons[5][13] = 
{
	"AimOffscreen",
	"Fire",
	"Cursor",
	"ToggleTurbo",
	"Pause"
	// AimOffScreen Fire,Cursor,ToggleTurbo,Pause
};

char snes_justifier_buttons[3][13] =
{
	"AimOffscreen",
	"Trigger",
	"Start"
	// AimOffscreen Trigger,Start
};

char snes_input_speeds[4][5] =
{
	"Var",
	"Slow",
	"Med",
	"Fast"
};

char snes_convert_input[3][16] =
{
	"AxisToPointer",
	"AxisToButtons",
	"ButtonToPointer"
};

char snes_pointer_dir[4][2] =
{
	"u",
	"d",
	"l",
	"r"
};

// MSB    Device Input Unk-   Player    Button
//        Type   Type  nown  Number    Mapping
// 0x1000 1111   1111  0000   1111 1111 1111 1111
enum
{
	k_HD  = 0x80000000,

	k_JP  = 0x01000000,
	k_MO  = 0x02000000,
	k_SS  = 0x04000000,
	k_LG  = 0x08000000,

	k_BT  = 0x00100000,
	k_PT  = 0x00200000,
	k_PS  = 0x00400000,

	k_C1  = 0x00000100,
	k_C2  = 0x00000200,
	k_C3  = 0x00000400,
	k_C4  = 0x00000800,
	k_C5  = 0x00001000,
	k_C6  = 0x00002000,
	k_C7  = 0x00004000,
	k_C8  = 0x00008000
};

enum
{
	k_SnesPad_base           = k_HD | k_BT | k_JP,
	k_SnesMouse_base         = k_HD | k_BT | k_MO,
	k_SnesScope_base         = k_HD | k_BT | k_SS,
	k_SnesJustfier_base      = k_HD | k_BT | k_LG,
	k_SnesMouse_Pointer_base = k_HD | k_PT | k_MO,

	k_SnesSuperscope_Pointer = k_HD | k_PT | k_SS | k_C1,
	k_SnesJustifier_Pointer  = k_HD | k_PT | k_LG | k_C1,

	// Pseudo Justifier
	k_SnesPseudoPtrBase      = k_HD | k_PS | k_LG | k_C2
};

#endif /*__PS2_CONTROLS_H__*/
