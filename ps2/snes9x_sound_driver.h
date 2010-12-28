#ifndef __SNES9X_SOUND_DRIVER_H__
#define __SNES9X_SOUND_DRIVER_H__

#include "port.h"

class S9xSoundDriver
{
	public:
		virtual void init (void) = 0;
		virtual void terminate (void) = 0;
		virtual bool8 open_device (void) = 0;
		virtual void start (void) = 0;
		virtual void stop (void) = 0;
		virtual void mix (void) = 0;
};

#endif /*__SNES9X_SOUND_DRIVER_H__*/
