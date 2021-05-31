#include "GPIO.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

//#define NODEBUG_PRINT
#include "debug_print.h"

#define BOUNCE_INTERVAL 50

GPIO::GPIO(unsigned char gpio, unsigned char settable){
    this->gpio = gpio;
    this->settable = settable;
    char buf[25];
    snprintf(buf,25,"gpio%d",gpio);
    gpioStr = strdup(buf);
}

void GPIO::loop(){
    if (settable) return;
    int v = digitalRead(gpio);
    if (v == status || millis()-changedOn < BOUNCE_INTERVAL) return;
    DEBUG_PRINT("%ul gpio=%d status=%d->%d\n",millis(),status,v);
    if (onChange) onChange(gpioStr, status, v);
    status = v;
    changedOn = millis();
}

void GPIO::printConfig(){
    CONSOLE("GPIO gpio=%d settable=%d status=%s\n",gpio,settable,status);
}