#include <SPI.h>

// #define FASTLED_ALL_PINS_HARDWARE_SPI
// #define FASTLED_ESP32_SPI_BUS HSPI

#include "RTOSConfig.h"
#include <FastLED.h>
#include "config.h"
#include "DisplayController.h"
#include "FrameBuffer.h"
#include "RotationManager.h"
#include "Renderer.h"
#include "Generators/SimpleLine.h"

static DisplayController* dc;

void setup()
{
    Serial.begin(1152000);
    delay(2500);
    dc = new DisplayController();
    dc->start();
}

void loop()
{

}