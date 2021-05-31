#include "GPIO.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#define NODEBUG_PRINT
#include "debug_print.h"

#define BOUNCE_INTERVAL 50

GPIO::GPIO(unsigned char gpio, unsigned char mode){
    this->gpio = gpio;
    this->settable = mode == OUTPUT;
    char buf[25];
    snprintf(buf,25,"%d",gpio);
    gpioStr = strdup(buf);
    pinMode(gpio,mode);
    status = settable ? 0 : digitalRead(gpio); // force change
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
    CONSOLE("GPIO id=%s gpio=%d settable=%d status=%d\n",gpioStr, gpio, settable, status);
}

void GPIO::publishStatus(){
    if (onChange) onChange(gpioStr, status, status);
}

void GPIO::set(unsigned char value){
    if (!settable) return;
    digitalWrite(gpio,value);
    DEBUG_PRINT("GPIO %s changed to %d\n",gpioStr,value);
}