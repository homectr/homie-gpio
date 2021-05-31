#include "config.h"

#include <pgmspace.h>

#ifdef USE_LITTLE_FS
#include <LittleFS.h>
#define BOARD_FS    LittleFS
#else
#include <FS.h>
#define BOARD_FS    SPIFFS
#endif

#include <ArduinoJson.h>
#include <Homie.h>

#include "GPIO.h"
#include "settings.h"

#define NODEBUG_PRINT
#include "debug_print.h"

//------------ FS
int initFS(){
    CONSOLE_PGM(PSTR("[init:FS] Mounting FS\n"));
    if (!BOARD_FS.begin()){
        CONSOLE_PGM(PSTR("[init:FS] >  FS FAILED TO MOUNT!\n"));
        CONSOLE_PGM(PSTR("[init:FS] >  Make sure to upload config.json to FS first!\n"));
    } else {
        DEBUG_PRINT("[init:FS] FS available\n");
    }

    return 1;

}


/**
 * Load configuration stored in EEPROM (BOARD_FS).
 * Configuration is stored in text formatted json file.
 * {
 *     "settings": {
 *         "monitor":[4,15],
 *         "set":[5]
 *     }
 * }
 */
int loadConfig(GPIOListItem** glist) {
    const char *module = "[init:ldcfg]";
    DynamicJsonDocument jdoc(2048);

    const char* f = LOCAL_CONFIG_FILE;
    DEBUG_PRINT("%s reading local config file=%s\n", module, f);
    
    if (!BOARD_FS.exists(f)) {
        DEBUG_PRINT("%s no configuration file\n", module);
        return 0;
    }
    
    File file = BOARD_FS.open(f,"r");
    auto err = deserializeJson(jdoc,file);
    file.close();

    if (err){
        CONSOLE_PGM(PSTR("%s invalid configuration file. error=%s\n"), module, err.c_str());
        return 0;
    }

    JsonVariant jv;
    JsonArray ja;
    JsonObject jroot = jdoc[F("settings")];

    CONSOLE_PGM(PSTR("%s Reading monitored gpios\n"), module);
    ja = jroot[F("monitor")].as<JsonArray>();
    for(JsonVariant v : ja) {
        unsigned int gn = v | 0;
        DEBUG_PRINT("%s   #%d", module, gn);
        if (gn) {
            GPIOListItem *g = new GPIOListItem();
            g->gpio = new GPIO(gn,INPUT_PULLUP);
            DEBUG_PRINT(" item=%X gpio=%X\n",g, g->gpio);
            g->next = *glist;
            *glist = g;
        }
    }

    CONSOLE_PGM(PSTR("%s Reading controlled gpios\n"), module);
    ja = jroot[F("control")].as<JsonArray>();
    for(JsonVariant v : ja) {
        unsigned int gn = v | 0;
        DEBUG_PRINT("%s   #%d", module, gn);
        if (gn) {
            GPIOListItem *g = new GPIOListItem();
            g->gpio = new GPIO(gn,OUTPUT);
            DEBUG_PRINT(" item=%X gpio=%X\n",g, g->gpio);
            g->next = *glist;
            *glist = g;
        }
    }    
    
    return 1;
}
