#ifndef __PS2_CONTROLS_H__
#define __PS2_CONTROLS_H__



//    MSB  Device    Player Button
//         Type      Number Mapping
// 0x 1000 1111 1111 1111   1111 1111 1111 1111

#define MOUSE_BUTTONS 3
#define MOUSE_AXES    2

#define PAD_BUTTONS 16
#define PAD_AXES     4

enum
{
	k_HD  = 0x80000000,

	k_DS  = 0x00100000,
	k_KB  = 0x00200000,
	k_MO  = 0x00400000,

	k_C1  = 0x00000000,
	k_C2  = 0x00010000,
	k_C3  = 0x00020000,
	k_C4  = 0x00030000,
	k_C5  = 0x00040000,
	k_C6  = 0x00050000,
	k_C7  = 0x00060000,
	k_C8  = 0x00070000
};

enum
{
	k_PS2_DS_BASE    = k_HD | k_DS,
	k_PS2_KB_BASE    = k_HD | k_KB,
	k_PS2_MO_BASE    = k_HD | k_MO,
};

enum
{
	k_Pseudo_Mouse1 = 0,
	k_Pseudo_Mouse2,
	k_Pseudo_SuperScope,
	k_Pseudo_Justifier1,
	k_Pseudo_Justifier2
};

#endif /*__PS2_CONTROLS_H__*/
