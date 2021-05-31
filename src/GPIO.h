#pragma once

using gpio_cb_t = void(*)(const char* gpioStr, int statusOld, int statusNew);

class GPIO {
    public:
        unsigned char gpio;
        char* gpioStr;
        unsigned char settable;
        unsigned long changedOn = 0;
        int status;
        gpio_cb_t onChange = 0;

    public:
        GPIO(unsigned char gpio, unsigned char settable);
        void loop();
        void printConfig();
        // set callback for gpio change
        void setOnChangeCB(gpio_cb_t cb){onChange = cb;};
        void publishStatus();
        void set(unsigned char value);
};

struct GPIOListItem {
    GPIO* gpio;
    GPIOListItem* next;
};
