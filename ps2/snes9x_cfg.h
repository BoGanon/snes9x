#ifndef __SNES9X_CFG_H__
#define __SNES9X_CFG_H__

#include <libconfig.h>
#include <cfg.h>

void S9xInitSettings(void);
void S9xParseCFG(config_t *config);
void S9xSaveSettingsToCFG(char *path);

#endif
