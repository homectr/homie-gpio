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

HomieNode node("gpio", "GPIO","gpio");  // node for manipulating gpios

bool configLoaded = false;

void onGpioChange(const char* gpioStr, int statusOld, int statusNew){
    DEBUG_PRINT("[gpioChanged] id=%s\n",gpioStr);
    // update Homie property
    if (Homie.isConnected()) node.setProperty(gpioStr).send(statusNew?"1":"0");
    Homie.getLogger() << millis() << " GPIO " << gpioStr << " " << statusOld << " -> " << statusNew << endl;
}

bool updateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value){
    DEBUG_PRINT("[updateHandler node=%s prop=%s val=%s\n",node.getId(),property.c_str(), value.c_str());
    if (strcmp(node.getId(),"gpio") != 0) return false;
    bool v = value == "true";
    GPIOListItem *i = GPIOS;
    while (i){
        if (property == i->gpio->gpioStr) { 
            i->gpio->set(v);
            Homie.getLogger() << millis() << " GPIO " << property.c_str() << " set to " << v << endl;
            return true;
        }
        i = i->next;
    }

    return false;
}

void onHomieEvent(const HomieEvent& event) {
  switch(event.type) {
    case HomieEventType::SENDING_STATISTICS:
      // Do whatever you want when statistics are sent in normal mode
      GPIOListItem *i = GPIOS;
        while (i) {
            i->gpio->publishStatus();
            i = i->next;
        }
      break;
  }
}

void setup() {
    Serial.begin(115200);
    Serial << endl << endl;

    initFS();

    Homie_setFirmware("Homie GPIO controller", "1.0.0");
    Homie.setGlobalInputHandler(updateHandler);
    Homie.onEvent(onHomieEvent);
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
        i=i->next;
    }

    if (!configLoaded && Homie.isConfigured()){
        loadConfig(&GPIOS);
        CONSOLE("%lu config loaded\n",millis());
        configLoaded = true;
        GPIOListItem *i = GPIOS;
        CONSOLE("%lu gpios:\n",millis());
        while (i) {
            DEBUG_PRINT("gpios adr=%X gpioadr=%X nextadr=%X\n",i, i->gpio, i->next);
            HomieInternals::PropertyInterface prop = node.advertise(i->gpio->gpioStr).setName(i->gpio->gpioStr).setDatatype("boolean");
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