#ifndef __PS2_SOUND_DRIVER_AUDSRV_H__
#define __PS2_SOUND_DRIVER_AUDSRV_H__

#include <snes9x_sound_driver.h>

#include <audsrv.h>

class S9xAudsrvSoundDriver : public S9xSoundDriver
{
	public:
		S9xAudsrvSoundDriver (void);

		void init (void);
		void terminate (void);
		bool8 open_device (void);
		void start (void);
		void stop (void);
		void mix (void);
		void samples_available (void);

	private:
		audsrv_fmt_t *format;
		uint8 *buffer;
		int buffer_size;

};

#endif /*__PS2_SOUND_DRIVER_AUDSRV_H__*/
