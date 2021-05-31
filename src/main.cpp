#include <Homie.h>
#include <errno.h>

#include "GPIO.h"
#include "settings.h"
#include "config.h"

#define NODEBUG_PRINT
#include "debug_print.h"

GPIOListItem *GPIOS;

const char* optsON = "open OPEN on ON 1";
const char* optsOFF = "closed CLOSED off OFF 0";
String opts = String(optsON) + " " + String(optsOFF);
unsigned char negativeOpts = strlen(optsON)+1; // start of negative options

HomieNode node("gpiomon", "GPIO monitor","gpiomon");  // node for manipulating gpios

bool configLoaded = false;

void onGpioChange(const char* gpioStr, int statusOld, int statusNew){
    DEBUG_PRINT("[gpioChanged] id=%s\n",gpioStr);
    // update Homie property
    if (Homie.isConnected()) node.setProperty(gpioStr).send(statusNew?"1":"0");
    Homie.getLogger() << millis() << " Gpio " << gpioStr << " " << statusOld << " -> " << statusNew << endl;
}

bool updateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value){
    DEBUG_PRINT("[updateHandler] node=%s pro=%s value=%s\n",node.getId(), property.c_str(), value.c_str());
    return true;
}

void setup() {
    Serial.begin(115200);
    Serial << endl << endl;

    initFS();

    Homie_setFirmware("Homie GPIO controller", "1.0.0");
    Homie.setGlobalInputHandler(updateHandler);
    Homie.setup();
}

#define CHECK_INTERVAL 2000
#define ALIVE_INTERVAL 30000
unsigned long lastCheck = millis()-1000000;
unsigned long alive = millis() - 1000000;

void loop() {
    Homie.loop();

    if (millis()-alive > (long)ALIVE_INTERVAL){
        CONSOLE("%lu alive\n",millis());
        alive=millis();
    }

    GPIOListItem *i = GPIOS;
    while (i){
        i->gpio->loop();
    }

    if (!configLoaded && Homie.isConfigured()){
        loadConfig(GPIOS);
        configLoaded = true;
        GPIOListItem *i = GPIOS;
        while (i) {
            HomieInternals::PropertyInterface prop = node.advertise(i->gpio->gpioStr).setName(i->gpio->gpioStr).setDatatype("integer");
            if (i->gpio->settable){
                prop.settable();
            } else {
                i->gpio->setOnChangeCB(onGpioChange);
            }

            i->gpio->printConfig();
            i = i->next;
        }
    }

}