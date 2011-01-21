static uint8 GetPPU_RAM (uint16 Address)
{
	return (OpenBus);
}

static uint8 GetPPU_RAM1 (uint16 Address)
{
	return (PPU.OpenBus1);
}
/*
static uint8 GetPPU_2102 (uint16 Address)
{
	return (uint8)(PPU.OAMAddr);
}

static uint8 GetPPU_2103 (uint16 Address)
{
	return (((PPU.OAMAddr >> 8) & 1) | (PPU.OAMPriorityRotation << 7));
}

static uint8 GetPPU_2116 (uint16 Address)
{
	return (uint8)(PPU.VMA.Address);
}

static uint8 GetPPU_2117 (uint16 Address)
{
	return (PPU.VMA.Address >> 8);
}

static uint8 GetPPU_2121 (uint16 Address)
{
	return (PPU.CGADD);
}
*/

static uint8 GetPPU_213x (uint16 Address)
{
	// 16bit x 8bit multiply read result.
	if (PPU.Need16x8Mulitply)
	{
		int32 r = (int32) PPU.MatrixA * (int32) (PPU.MatrixB >> 8);
		Memory.FillRAM[0x2134] = (uint8) r;
		Memory.FillRAM[0x2135] = (uint8)(r >> 8);
		Memory.FillRAM[0x2136] = (uint8)(r >> 16);
		PPU.Need16x8Mulitply = FALSE;
	}
	return (PPU.OpenBus1 = Memory.FillRAM[Address]);
}

static uint8 GetPPU_2137 (uint16 Address)
{
	S9xLatchCounters(0);
	return (OpenBus);
}

static uint8 GetPPU_2138 (uint16 Address)
{
	uint8 byte=0;

	if (PPU.OAMAddr & 0x100)
	{
		if (!(PPU.OAMFlip & 1))
			byte = PPU.OAMData[(PPU.OAMAddr & 0x10f) << 1];
		else
		{
			byte = PPU.OAMData[((PPU.OAMAddr & 0x10f) << 1) + 1];
			PPU.OAMAddr = (PPU.OAMAddr + 1) & 0x1ff;
			if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
			{
				PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
				IPPU.OBJChanged = TRUE;
			#ifdef DEBUGGER
				missing.sprite_priority_rotation = 1;
			#endif
			}
		}
	}
	else
	{
		if (!(PPU.OAMFlip & 1))
			byte = PPU.OAMData[PPU.OAMAddr << 1];
		else
		{
			byte = PPU.OAMData[(PPU.OAMAddr << 1) + 1];
			++PPU.OAMAddr;
			if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
			{
				PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
				IPPU.OBJChanged = TRUE;
			#ifdef DEBUGGER
				missing.sprite_priority_rotation = 1;
			#endif
			}
		}
	}

	PPU.OAMFlip ^= 1;
#ifdef DEBUGGER
	missing.oam_read = 1;
#endif
	return (PPU.OpenBus1 = byte);

}

static uint8 GetPPU_2139 (uint16 Address)
{
	uint8 byte=0;
#ifdef CORRECT_VRAM_READS
	byte = IPPU.VRAMReadBuffer & 0xff;
	if (!PPU.VMA.High)
	{
		if (PPU.VMA.FullGraphicCount)
		{
			uint32 addr = PPU.VMA.Address;
			uint32 rem = addr & PPU.VMA.Mask1;
			uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
			IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((address << 1) & 0xffff));
		}
		else
			IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));

			PPU.VMA.Address += PPU.VMA.Increment;
	}
#else
	if (IPPU.FirstVRAMRead)
		byte = Memory.VRAM[(PPU.VMA.Address << 1) & 0xffff];
	else
	if (PPU.VMA.FullGraphicCount)
	{
		uint32 addr = PPU.VMA.Address - 1;
		uint32 rem = addr & PPU.VMA.Mask1;
		uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
		byte = Memory.VRAM[((address << 1) - 2) & 0xffff];
	}
	else
		byte = Memory.VRAM[((PPU.VMA.Address << 1) - 2) & 0xffff];

	if (!PPU.VMA.High)
	{
		PPU.VMA.Address += PPU.VMA.Increment;
		IPPU.FirstVRAMRead = FALSE;
	}
#endif
#ifdef DEBUGGER
	missing.vram_read = 1;
#endif

	return (PPU.OpenBus1 = byte);
}

static uint8 GetPPU_213A (uint16 Address)
{
	uint8 byte=0;
#ifdef CORRECT_VRAM_READS
	byte = (IPPU.VRAMReadBuffer >> 8) & 0xff;
	if (PPU.VMA.High)
	{
		if (PPU.VMA.FullGraphicCount)
		{
			uint32 addr = PPU.VMA.Address;
			uint32 rem = addr & PPU.VMA.Mask1;
			uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
			IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((address << 1) & 0xffff));
		}
		else
			IPPU.VRAMReadBuffer = READ_WORD_ALIGNED(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));

		PPU.VMA.Address += PPU.VMA.Increment;
	}
#else
	if (IPPU.FirstVRAMRead)
		byte = Memory.VRAM[((PPU.VMA.Address << 1) + 1) & 0xffff];
	else
	if (PPU.VMA.FullGraphicCount)
	{
		uint32 addr = PPU.VMA.Address - 1;
		uint32 rem = addr & PPU.VMA.Mask1;
		uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
		byte = Memory.VRAM[((address << 1) - 1) & 0xffff];
	}
	else
		byte = Memory.VRAM[((PPU.VMA.Address << 1) - 1) & 0xffff];

	if (PPU.VMA.High)
	{
		PPU.VMA.Address += PPU.VMA.Increment;
		IPPU.FirstVRAMRead = FALSE;
	}
#endif
#ifdef DEBUGGER
	missing.vram_read = 1;
#endif

	return (PPU.OpenBus1 = byte);
}

static uint8 GetPPU_213B (uint16 Address)
{
	uint8 byte=0;

	if (PPU.CGFLIPRead)
		byte = (PPU.OpenBus2 & 0x80) | ((PPU.CGDATA[PPU.CGADD++] >> 8) & 0x7f);
	else
		byte = PPU.CGDATA[PPU.CGADD] & 0xff;
	PPU.CGFLIPRead ^= 1;
#ifdef DEBUGGER
	missing.cgram_read = 1;
#endif
	return (PPU.OpenBus2 = byte);
}

static uint8 GetPPU_213C (uint16 Address)
{
	uint8 byte=0;

	S9xTryGunLatch(false);
	if (PPU.HBeamFlip)
		byte = (PPU.OpenBus2 & 0xfe) | ((PPU.HBeamPosLatched >> 8) & 0x01);
	else
		byte = (uint8) PPU.HBeamPosLatched;
	PPU.HBeamFlip ^= 1;
#ifdef DEBUGGER
	missing.h_counter_read = 1;
#endif

	return (PPU.OpenBus2 = byte);
}

static uint8 GetPPU_213D (uint16 Address)
{
	uint8 byte = 0;
	S9xTryGunLatch(false);
	if (PPU.VBeamFlip)
		byte = (PPU.OpenBus2 & 0xfe) | ((PPU.VBeamPosLatched >> 8) & 0x01);
	else
		byte = (uint8) PPU.VBeamPosLatched;
	PPU.VBeamFlip ^= 1;
#ifdef DEBUGGER
	missing.v_counter_read = 1;
#endif

	return (PPU.OpenBus2 = byte);
}

static uint8 GetPPU_213E (uint16 Address)
{
	uint8 byte = 0;
	FLUSH_REDRAW();
	byte = (PPU.OpenBus1 & 0x10) | PPU.RangeTimeOver | Model->_5C77;
	return (PPU.OpenBus1 = byte);
}

static uint8 GetPPU_213F (uint16 Address)
{
	uint8 byte = 0;
	// NTSC/PAL and which field flags
	S9xTryGunLatch(false);
	PPU.VBeamFlip = PPU.HBeamFlip = 0;
	byte = (PPU.OpenBus2 & 0x20) | (Memory.FillRAM[0x213f] & 0xc0) | (Settings.PAL ? 0x10 : 0) | Model->_5C78;
	Memory.FillRAM[0x213f] &= ~0x40;

	return (PPU.OpenBus2 = byte);
}

// shouldn't be needed
static uint8 GetPPU_APUR (uint16 Address)
{
/*
	uint8 byte=0;

//	CPU.Flags |= DEBUG_MODE_FLAG;
#ifdef SPC700_SHUTDOWN	
	(IAPUuncached->APUExecuting) = Settings.APUEnabled;
	(IAPUuncached->WaitCounter)++;
#endif
	if (Settings.APUEnabled) {
#ifdef CPU_SHUTDOWN
//		CPU.WaitAddress = CPU.PCAtOpcodeStart;
#endif	
		if (SNESGameFixes.APU_OutPorts_ReturnValueFix && Address >= 0x2140 && Address <= 0x2143 && !CPU.V_Counter) {
			return (uint8)((Address & 1) ? ((rand() & 0xff00) >> 8) :  (rand() & 0xff));
		}
		return ((APUuncached->OutPorts) [Address & 3]);
	}

	switch (Settings.SoundSkipMethod) {
		case 0:
	  case 1:
			CPU.BranchSkip = TRUE;
			break;
	  case 2:
			break;
	  case 3:
			CPU.BranchSkip = TRUE;
			break;
	}
	if (Address & 3 < 2) {
		int r = rand ();
		if (r & 2) {		
			if (r & 4){	    
				return (Address & 3 == 1 ? 0xaa : 0xbb);
			} else {		    
				return ((r >> 3) & 0xff);
			}
		}
	} else {
		int r = rand ();
		if (r & 2) {
			return ((r >> 3) & 0xff);
		}
	}
	return (Memory.FillRAM[Address]);
*/
	return (OpenBus);
}

static uint8 GetPPU_2180 (uint16 Address)
{
	uint8 byte = 0;

	if (!CPU.InWRAMDMAorHDMA)
	{
		byte = Memory.RAM[PPU.WRAM++];
		PPU.WRAM &= 0x1ffff;
	}
	else
		byte = OpenBus;
#ifdef DEBUGGER
	missing.wram_read = 1;
#endif

	return (byte);
}

static uint8 GetPPU_2190 (uint16 Address) {
	return 1;
}

static uint8 GetPPU_0 (uint16 Address) {
	uint8 byte=0;
	return byte;
}

uint8 (*GetPPU[])(uint16 Address) = {
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  //0x2107
	GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  //0x210F
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  //0x2117
	GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  //0x211F
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  //0x2127
	GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  //0x212F
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_213x, GetPPU_213x, GetPPU_213x, GetPPU_2137, //0x2137
	GetPPU_2138, GetPPU_2139, GetPPU_213A, GetPPU_213B, GetPPU_213C, GetPPU_213D, GetPPU_213E, GetPPU_213F, //0x213F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2147
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x214F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2157
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x215F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2167
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x216F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2177
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x217F
	GetPPU_2180, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM
};
/*
GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,    //0x2187
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,    //0x218F
	GetPPU_RAM
};
*/
uint8 S9xGetPPU (uint16 Address)
{

	//not a real PPU reg
	if(Address < 0x2100)
		return (OpenBus); //treat as unmapped memory returning last byte on the bus

	if (CPU.InDMAorHDMA)
	{
		if (CPU.CurrentDMAorHDMAChannel >= 0 && !DMA[CPU.CurrentDMAorHDMAChannel].ReverseTransfer)
		{
			// S9xGetPPU() is called to read from DMA[].AAddress
			if ((Address & 0xff00) == 0x2100)
				// Cannot access to Address Bus B ($2100-$21FF) via (H)DMA
				return (OpenBus);
			else
				// $2200-$3FFF are connected to Address Bus A
				// SA1, SuperFX and SRTC are mapped here
				// I don't bother for now...
				return (OpenBus);
		}
		else
		{
			// S9xGetPPU() is called to write to $21xx
			// Take care of DMA wrapping
			if (Address > 0x21ff)
				Address = 0x2100 + (Address & 0xff);
		}
	}

	if ((Address & 0xffc0) == 0x2140) // APUIO0, APUIO1, APUIO2, APUIO3
		// read_port will run the APU until given APU time before reading value
		return (S9xAPUReadPort(Address & 3));
	else
	if (Address <= 0x2183){
		return GetPPU[Address - 0x2100](Address);
	}
	else
	{
		if (Settings.SuperFX)
		{
			if (Address >= 0x3000 && Address <= 0x32ff)
				return (S9xGetSuperFX(Address));
		}
		else
		if (Settings.SA1)
		{
			if (Address >= 0x2200)
				return (S9xGetSA1(Address));
		}
		else
		if (Settings.BS)
		{
			if (Address >= 0x2188 && Address <= 0x219f)
				return (S9xGetBSXPPU(Address));
		}
		else
		if (Settings.SRTC)
		{
			if (Address == 0x2800)
				return (S9xGetSRTC(Address));
		}
		else
		{
			switch (Address)
			{
				case 0x21c2:
					if (Model->_5C77 == 2)
						return (0x20);
					return (OpenBus);

				case 0x21c3:
					if (Model->_5C77 == 2)
						return (0);
					return (OpenBus);

				default:
					return (OpenBus);
			}
		}
	}

	// Not sure why this is needed
	return (OpenBus);

}
