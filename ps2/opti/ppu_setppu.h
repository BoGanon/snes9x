static void SetPPU_2100 (uint8 Byte, uint16 Address)
{
	// Brightness and screen blank bit
	if (Byte != Memory.FillRAM [0x2100])
	{
		FLUSH_REDRAW();

		if (PPU.Brightness != (Byte & 0xF))
		{
			IPPU.ColorsChanged = TRUE;
			IPPU.DirectColourMapsNeedRebuild = TRUE;
			PPU.Brightness = Byte & 0xF;
			S9xFixColourBrightness ();
			if (PPU.Brightness > IPPU.MaxBrightness)
				IPPU.MaxBrightness = PPU.Brightness;
		}
		if ((Memory.FillRAM[0x2100] & 0x80) != (Byte & 0x80))
		{
			IPPU.ColorsChanged = TRUE;
			PPU.ForcedBlanking = (Byte >> 7) & 1;
		}
		Memory.FillRAM[0x2100] = Byte;
	}

	if ((Memory.FillRAM[0x2100] & 0x80) && CPU.V_Counter == PPU.ScreenHeight + FIRST_VISIBLE_LINE)
	{
		PPU.OAMAddr = PPU.SavedOAMAddr;

		uint8 tmp = 0;
		if (PPU.OAMPriorityRotation)
			tmp = (PPU.OAMAddr & 0xfe) >> 1;
		if ((PPU.OAMFlip & 1) || PPU.FirstSprite != tmp)
		{
			PPU.FirstSprite = tmp;
			IPPU.OBJChanged = TRUE;
		}

		PPU.OAMFlip = 0;

		Memory.FillRAM[0x2100] = Byte;
	}

}

static void SetPPU_2101 (uint8 Byte, uint16 Address)
{
	// Sprite (OBJ) tile address
	if (Byte != Memory.FillRAM [0x2101])
	{
		FLUSH_REDRAW();
		PPU.OBJNameBase   = (Byte & 3) << 14;
		PPU.OBJNameSelect = ((Byte >> 3) & 3) << 13;
		PPU.OBJSizeSelect = (Byte >> 5) & 7;
		IPPU.OBJChanged = TRUE;

		Memory.FillRAM[0x2101] = Byte;
	}

}

static void SetPPU_2102 (uint8 Byte, uint16 Address)
{
	// Sprite write address (low)
	PPU.OAMAddr = ((Memory.FillRAM[0x2103] & 1) << 8) | Byte;
	PPU.OAMFlip = 2;
	PPU.OAMReadFlip = 0;
	PPU.SavedOAMAddr = PPU.OAMAddr;

	if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
	{
		PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
		IPPU.OBJChanged = TRUE;
#ifdef DEBUGGER
		missing.sprite_priority_rotation = 1;
#endif
	}

	Memory.FillRAM[0x2102] = Byte;
}

static void SetPPU_2103 (uint8 Byte, uint16 Address)
{
	PPU.OAMAddr = ((Byte & 1) << 8) | Memory.FillRAM[0x2102];
	PPU.OAMPriorityRotation = (Byte & 0x80) ? 1 : 0;

	if (PPU.OAMPriorityRotation)
	{
		if (PPU.FirstSprite != (PPU.OAMAddr >> 1))
		{
			PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
			IPPU.OBJChanged = TRUE;
		#ifdef DEBUGGER
			missing.sprite_priority_rotation = 1;
		#endif
		}
	}
	else
	{
		if (PPU.FirstSprite != 0)
		{
			PPU.FirstSprite = 0;
			IPPU.OBJChanged = TRUE;
		#ifdef DEBUGGER
			missing.sprite_priority_rotation = 1;
		#endif
		}
	}

	PPU.OAMFlip = 0;
	PPU.OAMReadFlip = 0;
	PPU.SavedOAMAddr = PPU.OAMAddr;

	Memory.FillRAM[0x2103] = Byte;

}

static void SetPPU_2104 (uint8 Byte, uint16 Address)
{
	// Sprite register write
	REGISTER_2104(Byte);

	Memory.FillRAM[0x2104] = Byte;
}

static void SetPPU_2105 (uint8 Byte, uint16 Address)
{
	// Screen mode (0 - 7), background tile sizes and background 3
	// priority
	if (Byte != Memory.FillRAM [0x2105])
	{
		FLUSH_REDRAW();
		PPU.BG[0].BGSize = (Byte >> 4) & 1;
		PPU.BG[1].BGSize = (Byte >> 5) & 1;
		PPU.BG[2].BGSize = (Byte >> 6) & 1;
		PPU.BG[3].BGSize = (Byte >> 7) & 1;
		PPU.BGMode = Byte & 7;
		// BJ: BG3Priority only takes effect if BGMode == 1 and the bit is set
		PPU.BG3Priority = ((Byte & 0x0f) == 0x09);
		if (PPU.BGMode == 5 || PPU.BGMode == 6)
			IPPU.Interlace = Memory.FillRAM[0x2133] & 1;
		else
			IPPU.Interlace = 0;
	#ifdef DEBUGGER
		missing.modes[PPU.BGMode] = 1;
	#endif

		Memory.FillRAM[0x2105] = Byte;
	}

}

static void SetPPU_2106 (uint8 Byte, uint16 Address)
{
	// Mosaic pixel size and enable
	if (Byte != Memory.FillRAM [0x2106])
	{
		FLUSH_REDRAW();
		PPU.MosaicStart = CPU.V_Counter;
		if (PPU.MosaicStart > PPU.ScreenHeight)
			PPU.MosaicStart = 0;
		PPU.Mosaic = (Byte >> 4) + 1;
		PPU.BGMosaic [0] = (Byte & 1);
		PPU.BGMosaic [1] = (Byte & 2);
		PPU.BGMosaic [2] = (Byte & 4);
		PPU.BGMosaic [3] = (Byte & 8);
	#ifdef DEBUGGER
		if ((Byte & 0xf0) && (Byte & 0x0f))
			missing.mosaic = 1;
	#endif

		Memory.FillRAM[0x2106] = Byte;
	}
}

static void SetPPU_2107 (uint8 Byte, uint16 Address)
{
	// [BG0SC]
	if (Byte != Memory.FillRAM [0x2107])
	{
		FLUSH_REDRAW();
		PPU.BG[0].SCSize = Byte & 3;
		PPU.BG[0].SCBase = (Byte & 0x7c) << 8;

		Memory.FillRAM[0x2107]=Byte;
	}
}

static void SetPPU_2108 (uint8 Byte, uint16 Address)
{
	// [BG1SC]
	if (Byte != Memory.FillRAM [0x2108])
	{
		FLUSH_REDRAW();
		PPU.BG[1].SCSize = Byte & 3;
		PPU.BG[1].SCBase = (Byte & 0x7c) << 8;

		Memory.FillRAM[0x2108]=Byte;
	}
}

static void SetPPU_2109 (uint8 Byte, uint16 Address)
{
	// [BG2SC]
	if (Byte != Memory.FillRAM [0x2109])
	{
		FLUSH_REDRAW();
		PPU.BG[2].SCSize = Byte & 3;
		PPU.BG[2].SCBase = (Byte & 0x7c) << 8;

		Memory.FillRAM[0x2109]=Byte;
	}
}

static void SetPPU_210A (uint8 Byte, uint16 Address)
{
	// [BG3SC]
	if (Byte != Memory.FillRAM [0x210a])
	{
		FLUSH_REDRAW();
		PPU.BG[3].SCSize = Byte & 3;
		PPU.BG[3].SCBase = (Byte & 0x7c) << 8;

		Memory.FillRAM[0x210a]=Byte;
	}
}

static void SetPPU_210B (uint8 Byte, uint16 Address)
{
	// [BG01NBA]
	if (Byte != Memory.FillRAM [0x210b])
	{
		FLUSH_REDRAW();
		PPU.BG[0].NameBase = (Byte & 7) << 12;
		PPU.BG[1].NameBase = ((Byte >> 4) & 7) << 12;

		Memory.FillRAM[0x210b]=Byte;
	}
}

static void SetPPU_210C (uint8 Byte, uint16 Address)
{
	// [BG23NBA]
	if (Byte != Memory.FillRAM [0x210c])
	{
		FLUSH_REDRAW();
		PPU.BG[2].NameBase = (Byte & 7) << 12;
		PPU.BG[3].NameBase = ((Byte >> 4) & 7) << 12;

		Memory.FillRAM[0x210c]=Byte;
	}
}


static void SetPPU_210D (uint8 Byte, uint16 Address)
{
	//PPU.BG[0].HOffset = ((PPU.BG[0].HOffset >> 8) & 0xff) | ((uint16) Byte << 8);

	PPU.BG[0].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[0].HOffset >> 8) & 7);
	PPU.M7HOFS = (Byte << 8) | PPU.M7byte;
	PPU.BGnxOFSbyte = Byte;
	PPU.M7byte = Byte;

	Memory.FillRAM[0x210d]=Byte;
}

static void SetPPU_210E (uint8 Byte, uint16 Address)
{
	//PPU.BG[0].VOffset = ((PPU.BG[0].VOffset >> 8) & 0xff) | ((uint16) Byte << 8);
	PPU.BG[0].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
	PPU.M7VOFS = (Byte << 8) | PPU.M7byte;
	PPU.BGnxOFSbyte = Byte;
	PPU.M7byte = Byte;

	Memory.FillRAM[0x210e]=Byte;
}

static void SetPPU_210F (uint8 Byte, uint16 Address)
{
	//PPU.BG[1].HOffset = ((PPU.BG[1].HOffset >> 8) & 0xff) | ((uint16) Byte << 8);
	PPU.BG[1].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[1].HOffset >> 8) & 7);
	PPU.BGnxOFSbyte = Byte;

	Memory.FillRAM[0x210f]=Byte;
}

static void SetPPU_2110 (uint8 Byte, uint16 Address)
{
	//PPU.BG[1].VOffset = ((PPU.BG[1].VOffset >> 8) & 0xff) | ((uint16) Byte << 8);
	PPU.BG[1].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
	PPU.BGnxOFSbyte = Byte;

	Memory.FillRAM[0x2110]=Byte;
}

static void SetPPU_2111 (uint8 Byte, uint16 Address)
{
	//PPU.BG[2].HOffset = ((PPU.BG[2].HOffset >> 8) & 0xff) | ((uint16) Byte << 8);
	PPU.BG[2].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[2].HOffset >> 8) & 7);
	PPU.BGnxOFSbyte = Byte;

	Memory.FillRAM[0x2111]=Byte;
}

static void SetPPU_2112 (uint8 Byte, uint16 Address)
{
	//PPU.BG[2].VOffset = ((PPU.BG[2].VOffset >> 8) & 0xff) | ((uint16) Byte << 8);
	PPU.BG[2].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
	PPU.BGnxOFSbyte = Byte;

	Memory.FillRAM[0x2112]=Byte;
}

static void SetPPU_2113 (uint8 Byte, uint16 Address)
{
	//PPU.BG[3].HOffset = ((PPU.BG[3].HOffset >> 8) & 0xff) | ((uint16) Byte << 8);
	PPU.BG[3].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[3].HOffset >> 8) & 7);
	PPU.BGnxOFSbyte = Byte;

	Memory.FillRAM[0x2113]=Byte;
}

static void SetPPU_2114 (uint8 Byte, uint16 Address)
{
	//PPU.BG[3].VOffset = ((PPU.BG[3].VOffset >> 8) & 0xff) | ((uint16) Byte << 8);
	PPU.BG[3].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
	PPU.BGnxOFSbyte = Byte;

	Memory.FillRAM[0x2114]=Byte;
}

static void SetPPU_2115 (uint8 Byte, uint16 Address)
{
	// VRAM byte/word access flag and increment
	PPU.VMA.High = (Byte & 0x80) == 0 ? FALSE : TRUE;
	switch (Byte & 3)
	{
		case 0: PPU.VMA.Increment = 1;   break;
		case 1: PPU.VMA.Increment = 32;  break;
		case 2: PPU.VMA.Increment = 128; break;
		case 3: PPU.VMA.Increment = 128; break;
	}

	if (Byte & 0x0c)
	{
		static uint16 Shift[4]    = { 0, 5, 6, 7 };
		static uint16 IncCount[4] = { 0, 32, 64, 128 };

		uint8 i = (Byte & 0x0c) >> 2;
		PPU.VMA.FullGraphicCount = IncCount[i];
		PPU.VMA.Mask1 = IncCount[i] * 8 - 1;
		PPU.VMA.Shift = Shift[i];
	#ifdef DEBUGGER
		missing.vram_full_graphic_inc = (Byte & 0x0c) >> 2;
	#endif
	}
	else
		PPU.VMA.FullGraphicCount = 0;

#ifdef DEBUGGER
	if (Byte & 3)
		missing.vram_inc = Byte & 3;
#endif

	Memory.FillRAM[0x2115]=Byte;
}

static void SetPPU_2116 (uint8 Byte, uint16 Address)
{
	// VRAM read/write address (low)
	PPU.VMA.Address &= 0xFF00;
	PPU.VMA.Address |= Byte;
#ifdef CORRECT_VRAM_READS
	if (PPU.VMA.FullGraphicCount)
	{
		uint32 addr = PPU.VMA.Address;
		uint32 rem = addr & PPU.VMA.Mask1;
		uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
		IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((address << 1) & 0xffff));
	}
	else
		IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));
#else
	IPPU.FirstVRAMRead = TRUE;
#endif

	Memory.FillRAM[0x2116]=Byte;
}

static void SetPPU_2117 (uint8 Byte, uint16 Address)
{
// VRAM read/write address (high)
	PPU.VMA.Address &= 0x00FF;
	PPU.VMA.Address |= Byte << 8;

#ifdef CORRECT_VRAM_READS
	if (PPU.VMA.FullGraphicCount)
	{
		uint32 addr = PPU.VMA.Address;
		uint32 rem = addr & PPU.VMA.Mask1;
		uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
		IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((address << 1) & 0xffff));
	}
	else
		IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));
#else
	IPPU.FirstVRAMRead = TRUE;
#endif

	Memory.FillRAM[0x2117]=Byte;
}

static void SetPPU_2118 (uint8 Byte, uint16 Address)
{
	// VRAM write data (low)
#ifndef CORRECT_VRAM_READS
	IPPU.FirstVRAMRead = TRUE;
#endif
	REGISTER_2118(Byte);

	Memory.FillRAM[0x2118]=Byte;
}

static void SetPPU_2119 (uint8 Byte, uint16 Address)
{
	// VRAM write data (high)
#ifndef CORRECT_VRAM_READS
	IPPU.FirstVRAMRead = TRUE;
#endif
	REGISTER_2119(Byte);

	Memory.FillRAM[0x2119]=Byte;
}

static void SetPPU_211A (uint8 Byte, uint16 Address)
{
	// Mode 7 outside rotation area display mode and flipping
	if (Byte != Memory.FillRAM [0x211a])
	{
		FLUSH_REDRAW();
		PPU.Mode7Repeat = Byte >> 6;
		if (PPU.Mode7Repeat == 1)
		{
			PPU.Mode7Repeat = 0;
		}
		PPU.Mode7VFlip = (Byte & 2) >> 1;
		PPU.Mode7HFlip = Byte & 1;

		Memory.FillRAM[0x211a]=Byte;
	}
}

static void SetPPU_211B (uint8 Byte, uint16 Address)
{
	// Mode 7 matrix A (low & high)
	PPU.MatrixA = PPU.M7byte | (Byte << 8);
	PPU.Need16x8Mulitply = TRUE;
	PPU.M7byte = Byte;

	Memory.FillRAM[0x211b]=Byte;
}

static void SetPPU_211C (uint8 Byte, uint16 Address)
{
	// Mode 7 matrix B (low & high)
	PPU.MatrixB = PPU.M7byte | (Byte << 8);
	PPU.Need16x8Mulitply = TRUE;
	PPU.M7byte = Byte;

	Memory.FillRAM[0x211c]=Byte;
}

static void SetPPU_211D (uint8 Byte, uint16 Address)
{
	// Mode 7 matrix C (low & high)
	PPU.MatrixC = PPU.M7byte | (Byte << 8);
	PPU.M7byte = Byte;

	Memory.FillRAM[0x211d]=Byte;
}

static void SetPPU_211E (uint8 Byte, uint16 Address)
{
	// Mode 7 matrix D (low & high)
	PPU.MatrixD = PPU.M7byte | (Byte << 8);
	PPU.M7byte = Byte;

	Memory.FillRAM[0x211e]=Byte;
}

static void SetPPU_211F (uint8 Byte, uint16 Address)
{
	// Mode 7 centre of rotation X (low & high)
	PPU.CentreX = PPU.M7byte | (Byte << 8);
	PPU.M7byte = Byte;

	Memory.FillRAM[0x211f]=Byte;
}

static void SetPPU_2120 (uint8 Byte, uint16 Address)
{
	// Mode 7 centre of rotation Y (low & high)
	PPU.CentreY = PPU.M7byte | (Byte << 8);
	PPU.M7byte = Byte;

	Memory.FillRAM[0x2120]=Byte;
}

static void SetPPU_2121 (uint8 Byte, uint16 Address)
{
	// CG-RAM address
	PPU.CGFLIP = 0;
	PPU.CGFLIPRead = 0;
	PPU.CGADD = Byte;

	Memory.FillRAM[0x2121]=Byte;
}

static void SetPPU_2122 (uint8 Byte, uint16 Address)
{
	REGISTER_2122(Byte);

	Memory.FillRAM[0x2122]=Byte;
}

static void SetPPU_2123 (uint8 Byte, uint16 Address)
{
	// Window 1 and 2 enable for backgrounds 1 and 2
	if (Byte != Memory.FillRAM [0x2123])
	{
		FLUSH_REDRAW();
		PPU.ClipWindow1Enable [0] = !!(Byte & 0x02);
		PPU.ClipWindow1Enable [1] = !!(Byte & 0x20);
		PPU.ClipWindow2Enable [0] = !!(Byte & 0x08);
		PPU.ClipWindow2Enable [1] = !!(Byte & 0x80);
		PPU.ClipWindow1Inside [0] = !(Byte & 0x01);
		PPU.ClipWindow1Inside [1] = !(Byte & 0x10);
		PPU.ClipWindow2Inside [0] = !(Byte & 0x04);
		PPU.ClipWindow2Inside [1] = !(Byte & 0x40);
		PPU.RecomputeClipWindows = TRUE;
	#ifdef DEBUGGER
		if (Byte & 0x80)
			missing.window2[1] = 1;
		if (Byte & 0x20)
			missing.window1[1] = 1;
		if (Byte & 0x08)
			missing.window2[0] = 1;
		if (Byte & 0x02)
			missing.window1[0] = 1;
	#endif
		Memory.FillRAM[0x2123]=Byte;
	}
}

static void SetPPU_2124 (uint8 Byte, uint16 Address)
{
	// Window 1 and 2 enable for backgrounds 3 and 4
	if (Byte != Memory.FillRAM [0x2124])
	{
		FLUSH_REDRAW();
		PPU.ClipWindow1Enable [2] = !!(Byte & 0x02);
		PPU.ClipWindow1Enable [3] = !!(Byte & 0x20);
		PPU.ClipWindow2Enable [2] = !!(Byte & 0x08);
		PPU.ClipWindow2Enable [3] = !!(Byte & 0x80);
		PPU.ClipWindow1Inside [2] = !(Byte & 0x01);
		PPU.ClipWindow1Inside [3] = !(Byte & 0x10);
		PPU.ClipWindow2Inside [2] = !(Byte & 0x04);
		PPU.ClipWindow2Inside [3] = !(Byte & 0x40);
		PPU.RecomputeClipWindows = TRUE;
	#ifdef DEBUGGER
		if (Byte & 0x80)
			missing.window2[3] = 1;
		if (Byte & 0x20)
			missing.window1[3] = 1;
		if (Byte & 0x08)
			missing.window2[2] = 1;
		if (Byte & 0x02)
			missing.window1[2] = 1;
	#endif

		Memory.FillRAM[0x2124]=Byte;
	}
}

static void SetPPU_2125 (uint8 Byte, uint16 Address)
{
	// Window 1 and 2 enable for objects and colour window
	if (Byte != Memory.FillRAM [0x2125])
	{
		FLUSH_REDRAW();
		PPU.ClipWindow1Enable [4] = !!(Byte & 0x02);
		PPU.ClipWindow1Enable [5] = !!(Byte & 0x20);
		PPU.ClipWindow2Enable [4] = !!(Byte & 0x08);
		PPU.ClipWindow2Enable [5] = !!(Byte & 0x80);
		PPU.ClipWindow1Inside [4] = !(Byte & 0x01);
		PPU.ClipWindow1Inside [5] = !(Byte & 0x10);
		PPU.ClipWindow2Inside [4] = !(Byte & 0x04);
		PPU.ClipWindow2Inside [5] = !(Byte & 0x40);
		PPU.RecomputeClipWindows = TRUE;
#ifdef DEBUGGER
		if (Byte & 0x80)
			missing.window2[5] = 1;
		if (Byte & 0x20)
			missing.window1[5] = 1;
		if (Byte & 0x08)
			missing.window2[4] = 1;
		if (Byte & 0x02)
			missing.window1[4] = 1;
#endif

		Memory.FillRAM[0x2125]=Byte;
	}
}

static void SetPPU_2126 (uint8 Byte, uint16 Address)
{
	// Window 1 left position
	if (Byte != Memory.FillRAM [0x2126])
	{
		FLUSH_REDRAW();
		PPU.Window1Left = Byte;
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x2126]=Byte;
	}
}

static void SetPPU_2127 (uint8 Byte, uint16 Address)
{
	// Window 1 right position
	if (Byte != Memory.FillRAM [0x2127])
	{
		FLUSH_REDRAW();
		PPU.Window1Right = Byte;
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x2127]=Byte;
	}
}

static void SetPPU_2128 (uint8 Byte, uint16 Address)
{
	// Window 2 left position
	if (Byte != Memory.FillRAM [0x2128])
	{
		FLUSH_REDRAW();
		PPU.Window2Left = Byte;
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x2128]=Byte;
	}
}

static void SetPPU_2129 (uint8 Byte, uint16 Address)
{
	// Window 2 right position
	if (Byte != Memory.FillRAM [0x2129])
	{
		FLUSH_REDRAW();
		PPU.Window2Right = Byte;
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x2129]=Byte;
	}
}

static void SetPPU_212A (uint8 Byte, uint16 Address)
{
	// Windows 1 & 2 overlap logic for backgrounds 1 - 4
	if (Byte != Memory.FillRAM [0x212a])
	{
		FLUSH_REDRAW();
		PPU.ClipWindowOverlapLogic [0] = (Byte & 0x03);
		PPU.ClipWindowOverlapLogic [1] = (Byte & 0x0c) >> 2;
		PPU.ClipWindowOverlapLogic [2] = (Byte & 0x30) >> 4;
		PPU.ClipWindowOverlapLogic [3] = (Byte & 0xc0) >> 6;
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x212a]=Byte;
	}
}

static void SetPPU_212B (uint8 Byte, uint16 Address)
{
	// Windows 1 & 2 overlap logic for objects and colour window
	if (Byte != Memory.FillRAM [0x212b])
	{
		FLUSH_REDRAW();
		PPU.ClipWindowOverlapLogic [4] = Byte & 0x03;
		PPU.ClipWindowOverlapLogic [5] = (Byte & 0x0c) >> 2;
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x212b]=Byte;
	}
}

static void SetPPU_212C (uint8 Byte, uint16 Address)
{
	// Main screen designation (backgrounds 1 - 4 and objects)
	if (Byte != Memory.FillRAM [0x212c])
	{
		FLUSH_REDRAW();
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x212c]=Byte;
		return; 
	}
}

static void SetPPU_212D (uint8 Byte, uint16 Address)
{
	// Sub-screen designation (backgrounds 1 - 4 and objects)
	if (Byte != Memory.FillRAM [0x212d])
	{
		FLUSH_REDRAW();
		PPU.RecomputeClipWindows = TRUE;
	
	#ifdef DEBUGGER
		if (Byte & 0x1f)
			missing.subscreen = 1;
	#endif

		Memory.FillRAM[0x212d]=Byte;
		return;
	}
}

static void SetPPU_212E (uint8 Byte, uint16 Address)
{
	// Window mask designation for main screen ?
	if (Byte != Memory.FillRAM [0x212e])
	{
		FLUSH_REDRAW();
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x212e]=Byte;
	}
}

static void SetPPU_212F (uint8 Byte, uint16 Address)
{
	// Window mask designation for sub-screen ?
	if (Byte != Memory.FillRAM [0x212f])
	{
		FLUSH_REDRAW();
		PPU.RecomputeClipWindows = TRUE;

		Memory.FillRAM[0x212f]=Byte;
	}
}

static void SetPPU_2130 (uint8 Byte, uint16 Address)
{
	// Fixed colour addition or screen addition
	if (Byte != Memory.FillRAM [0x2130])
	{
		FLUSH_REDRAW();
		PPU.RecomputeClipWindows = TRUE;
	#ifdef DEBUGGER
		if ((Byte & 1) && (PPU.BGMode == 3 || PPU.BGMode == 4 || PPU.BGMode == 7))
			missing.direct = 1;
	#endif

		Memory.FillRAM[0x2130]=Byte;
	}
}

static void SetPPU_2131 (uint8 Byte, uint16 Address)
{
	// Colour addition or subtraction select
	if (Byte != Memory.FillRAM[0x2131])
	{
		FLUSH_REDRAW();
		// Backgrounds 1 - 4, objects and backdrop colour add/sub enable
	#ifdef DEBUGGER
		if (Byte & 0x80)
		{
			// Subtract
			if (Memory.FillRAM[0x2130] & 0x02)
				missing.subscreen_sub = 1;
			else
				missing.fixed_colour_sub = 1;
		}
		else
		{
			// Addition
			if (Memory.FillRAM[0x2130] & 0x02)
				missing.subscreen_add = 1;
			else
				missing.fixed_colour_add = 1;
		}
	#endif

		Memory.FillRAM[0x2131] = Byte;
	}
}

static void SetPPU_2132 (uint8 Byte, uint16 Address)
{
	if (Byte != Memory.FillRAM [0x2132])
	{
		FLUSH_REDRAW();

		// Colour data for fixed colour addition/subtraction
		if (Byte & 0x80)
			PPU.FixedColourBlue  = Byte & 0x1f;
		if (Byte & 0x40)
			PPU.FixedColourGreen = Byte & 0x1f;
		if (Byte & 0x20)
			PPU.FixedColourRed   = Byte & 0x1f;

		Memory.FillRAM[0x2132]=Byte;
	}
}

static void SetPPU_2133 (uint8 Byte, uint16 Address)
{
	// Screen settings
	if (Byte != Memory.FillRAM [0x2133])
	{

		if ((Memory.FillRAM[0x2133] ^ Byte) & 8)
		{
			FLUSH_REDRAW();
			IPPU.PseudoHires = Byte & 8;
		}

		if (Byte & 0x04)
		{
			PPU.ScreenHeight = SNES_HEIGHT_EXTENDED;
			if (IPPU.DoubleHeightPixels)
				IPPU.RenderedScreenHeight = PPU.ScreenHeight << 1;
			else
				IPPU.RenderedScreenHeight = PPU.ScreenHeight;
		#ifdef DEBUGGER
			missing.lines_239 = 1;
		#endif
		}
		else
			PPU.ScreenHeight = SNES_HEIGHT;

		if ((Memory.FillRAM[0x2133] ^ Byte) & 3)
		{
			FLUSH_REDRAW();
			if ((Memory.FillRAM[0x2133] ^ Byte) & 2)
				IPPU.OBJChanged = TRUE;
			if (PPU.BGMode == 5 || PPU.BGMode == 6)
				IPPU.Interlace = Byte & 1;

			IPPU.InterlaceOBJ = Byte & 2;
		}
	#ifdef DEBUGGER
		if (Byte & 0x40)
			missing.mode7_bgmode = 1;
		if (Byte & 0x08)
			missing.pseudo_512 = 1;
		if (Byte & 0x02)
			missing.sprite_double_height = 1;
		if (Byte & 0x01)
			missing.interlace = 1;
	#endif

		Memory.FillRAM[0x2133]=Byte;
	}
}

static void SetPPU_NOP (uint8 Byte, uint16 Address)
{
/*
case 0x2134:
    case 0x2135:
    case 0x2136:
	// Matrix 16bit x 8bit multiply result (read-only)
	return;

    case 0x2137:
	// Software latch for horizontal and vertical timers (read-only)
	return;
    case 0x2138:
	// OAM read data (read-only)
	return;
    case 0x2139:
    case 0x213a:
	// VRAM read data (read-only)
	return;
    case 0x213b:
	// CG-RAM read data (read-only)
	return;
    case 0x213c:
    case 0x213d:
	// Horizontal and vertical (low/high) read counter (read-only)
	return;
    case 0x213e:
	// PPU status (time over and range over)
	return;
    case 0x213f:
	// NTSC/PAL select and field (read-only)
	return;
*/
}

static void SetPPU_APU (uint8 Byte, uint16 Address)
{

	S9xAPUWritePort(Address & 3, Byte);
	Memory.FillRAM [Address] = Byte;

}

static void SetPPU_2180 (uint8 Byte, uint16 Address)
{
	if (!CPU.InWRAMDMAorHDMA)
		REGISTER_2180(Byte);
}


static void SetPPU_2181 (uint8 Byte, uint16 Address)
{
	if (!CPU.InWRAMDMAorHDMA)
	{
		PPU.WRAM &= 0x1FF00;
		PPU.WRAM |= Byte;
	}
}

static void SetPPU_2182 (uint8 Byte, uint16 Address)
{
	if (!CPU.InWRAMDMAorHDMA)
	{
		PPU.WRAM &= 0x100FF;
		PPU.WRAM |= Byte << 8;
	}
}

static void SetPPU_2183 (uint8 Byte, uint16 Address)
{
	if (!CPU.InWRAMDMAorHDMA)
	{
		PPU.WRAM &= 0x0FFFF;
		PPU.WRAM |= Byte << 16;
		PPU.WRAM &= 0x1FFFF;
	}
}

static void (*SetPPU[])(uint8 Byte, uint16 Address) = {
	SetPPU_2100, SetPPU_2101, SetPPU_2102, SetPPU_2103, SetPPU_2104, SetPPU_2105, SetPPU_2106, SetPPU_2107,
	SetPPU_2108, SetPPU_2109, SetPPU_210A, SetPPU_210B, SetPPU_210C, SetPPU_210D, SetPPU_210E, SetPPU_210F,
	SetPPU_2110, SetPPU_2111, SetPPU_2112, SetPPU_2113, SetPPU_2114, SetPPU_2115, SetPPU_2116, SetPPU_2117,
	SetPPU_2118, SetPPU_2119, SetPPU_211A, SetPPU_211B, SetPPU_211C, SetPPU_211D, SetPPU_211E, SetPPU_211F,
	SetPPU_2120, SetPPU_2121, SetPPU_2122, SetPPU_2123, SetPPU_2124, SetPPU_2125, SetPPU_2126, SetPPU_2127,
	SetPPU_2128, SetPPU_2129, SetPPU_212A, SetPPU_212B, SetPPU_212C, SetPPU_212D, SetPPU_212E, SetPPU_212F,
	SetPPU_2130, SetPPU_2131, SetPPU_2132, SetPPU_2133, SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP, 
	SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP,  SetPPU_NOP, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU,  SetPPU_APU, 
	SetPPU_2180, SetPPU_2181, SetPPU_2182, SetPPU_2183
};

void S9xSetPPU (uint8 Byte, uint16 Address)
{
//    fprintf(stderr, "%03d: %02x to %04x\n", CPU.V_Counter, Byte, Address);
	if (CPU.InDMAorHDMA)
	{
		if (CPU.CurrentDMAorHDMAChannel >= 0 && DMA[CPU.CurrentDMAorHDMAChannel].ReverseTransfer)
		{
			// S9xSetPPU() is called to write to DMA[].AAddress
			if ((Address & 0xff00) == 0x2100)
			{
				// Cannot access to Address Bus B ($2100-$21ff) via (H)DMA
				return;
			}
			else
			{
				// 0x2000-0x3FFF is connected to Address Bus A
				// SA1, SuperFX and SRTC are mapped here
				// I don't bother for now...
				return;
			}
		}
		else
		{
			// S9xSetPPU() is called to read from $21xx
			// Take care of DMA wrapping
			if (Address > 0x21ff)
				Address = 0x2100 + (Address & 0xff);
		}
	}

#ifdef DEBUGGER
	if (CPU.InHDMA)
		S9xTraceFormattedMessage("--- HDMA PPU %04X -> %02X", Address, Byte);
#endif

	//if ((Address & 0xffc0) == 0x2140) // APUIO0, APUIO1, APUIO2, APUIO3
		// write_port will run the APU until given clock before writing value
		//S9xAPUWritePort(Address & 3, Byte);
	//else
	if (Address <= 0x2183)
	{
		SetPPU[Address - 0x2100]( Byte, Address );
		return;
	}
	else
	{
		if (Settings.SuperFX)
		{
			if (Address >= 0x3000 && Address <= 0x32ff)
				S9xSetSuperFX(Byte, Address);
			return;
		}
		else
		if (Settings.SA1)
		{
			if ( Address >= 0x2200)
			{
				if ( Address <= 0x23ff)
					S9xSetSA1(Byte, Address);
				else
					Memory.FillRAM[Address] = Byte;
			}
			return;
		}
		else
		if (Settings.BS)
		{
			if (Address >= 0x2188 && Address <= 0x219f)
				S9xSetBSXPPU(Byte, Address);
		}
		else
		if (Settings.SRTC)
		{
			if (Address == 0x2801)
				S9xSetSRTC(Byte, Address);
	#ifdef DEBUGGER
			else
			{
				missing.unknownppu_write = Address;
				if (Settings.TraceUnknownRegisters)
				{
					sprintf(String, "Unknown register write: $%02X->$%04X\n", Byte, Address);
					S9xMessage(S9X_TRACE, S9X_PPU_TRACE, String);
				}
			}
	#endif
		}
	}

	Memory.FillRAM[Address] = Byte;

}
