#pragma once

#include "GPIO.h"

#define LOCAL_CONFIG_FILE   "/homie/settings.json"

int initFS();
int loadConfig(GPIOListItem** glist);
