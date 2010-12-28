#include <ps2_sound_driver_audsrv.h>

#include <apu.h>

static void audsrv_samples_available(void *data)
{
	((S9xAudsrvSoundDriver *)data)->samples_available();
}

S9xAudsrvSoundDriver::S9xAudsrvSoundDriver(void)
{

	format = NULL;
	buffer = NULL;
	buffer_size = 0;

	return;

}

void S9xAudsrvSoundDriver::init(void)
{
	return;
}

void S9xAudsrvSoundDriver::terminate(void)
{
	stop ();

	S9xSetSamplesAvailableCallback(NULL, NULL);

	if (format)
	{
		free(format);
	}

	if (buffer)
	{
		delete[] buffer;
		buffer = NULL;
	}

	return;

}

void S9xAudsrvSoundDriver::start(void)
{
	// hardcoded volume
	audsrv_set_volume(100);
	return;
}

void S9xAudsrvSoundDriver::stop(void)
{
	audsrv_set_volume(0);
	return;
}

bool8 S9xAudsrvSoundDriver::open_device(void)
{

	format = (audsrv_fmt_t*)malloc(sizeof(audsrv_fmt_t));

	if (!format)
	{
		return FALSE;
	}

	format->channels = Settings.Stereo ? 2 : 1;
	format->bits = Settings.SixteenBitSound ? 16 : 8;
	format->freq = Settings.SoundPlaybackRate;

	if (!audsrv_set_format(format))
	{
		printf ("OK\n");
	}
	else
	{
		return FALSE;
	}

	// hardcoded volume
	audsrv_set_volume(100);

	S9xSetSamplesAvailableCallback(audsrv_samples_available, this);

	return TRUE;

}

void S9xAudsrvSoundDriver::mix(void)
{
	return;
}

void S9xAudsrvSoundDriver::samples_available(void)
{

	int samples, bytes;

	S9xFinalizeSamples ();

	samples = S9xGetSampleCount();
	bytes = (samples << (Settings.SixteenBitSound ? 1 : 0));

	if (bytes <= 128)
		return;

	if (buffer_size < bytes || buffer == NULL)
	{
		delete[] buffer;
		buffer = new uint8[bytes];
		buffer_size = bytes;
	}

	S9xMixSamples (buffer, samples);

	if (Settings.TurboMode)
		return;

	audsrv_wait_audio(bytes);
	audsrv_play_audio((const char*)buffer,bytes);

	return;

}
