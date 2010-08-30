#include <controls.h>
#include <conffile.h>

// conffile.h
void S9xParsePortConfig(ConfigFile &, int pass)
{
}

// controls.h
void S9xHandlePortCommand (s9xcommand_t cmd, int16 data1, int16 data2)
{
}

bool S9xPollButton (uint32 id, bool *pressed)
{
	return false;
}

bool S9xPollPointer (uint32 id, int16 *x, int16 *y)
{
	return false;
}

bool S9xPollAxis (uint32 id, int16 *value)
{
	return false;
}
