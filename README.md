# Homie 3.0 compatible GPIO monitor+controller

Use cases: monitoring and triggering digital inputs/outputs.

## Requirements
* Arduino compatible board (tested with ESP8266 and ESP32)
* Platformio

## Configuration
There are two configuration files required to be present on device SPIFFS:
1. homie/config.json - see https://homieiot.github.io/homie-esp8266/docs/3.0.1/configuration/json-configuration-file/
1. homie/settings.json - see below

### settings.json
Specifies gpios to be monitored (read) and controlled (write).
> Monitored gpios are automatically pulled-up upon device boot, so monitored contact should ground them. 
> This also means that 1/true => open contact, 0/false => closed contact

> Controlled gpios will get 3.3V when witched to ON.

```
{
    "settings": {
        "monitor":[4,15],
        "control":[5]
    }
}
```

### Uploading configuration files
`config.json` can be uploaded using API (see https://homieiot.github.io/homie-esp8266/docs/3.0.1/configuration/http-json-api/) or uploaded directly.

Both file can be uploaded directly from Platformio.
1. store files to project folder `data/homie`
1. run file storage upload `pio run -t uploadfs`


