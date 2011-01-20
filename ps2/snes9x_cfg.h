#ifndef __SNES9X_CFG_H__
#define __SNES9X_CFG_H__

#include <libconfig.h>
#include <cfg.h>

extern int buffer_ms;
extern int lag_ms;

#define SNES9X_SECTION "Snes9X"

// Initializes Settings values
void S9xInitSettings(void);

// Adds settings to an existing configuration
void S9xAddSettingsToCFG(config_t *config);

// Loads and parses a configuration file
void S9xLoadCFG(char *path);

// Saves just the Snes9x configuration file
void S9xSaveCFG(char *path);

#endif
