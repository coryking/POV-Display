#include <SPI.h>

#include "RTOSConfig.h"
#include "FastLEDConfig.h"
#include "config.h"
#include "DisplayController.h"
#include "FrameBuffer.h"
#include "RotationManager.h"
#include "Renderer.h"
#include "Generators/SimpleLine.h"

static DisplayController* dc;

void setup()
{
    Serial.begin(115200);
    esp_log_level_set("*", ESP_LOG_VERBOSE);
    delay(2500);
    dc = new DisplayController();
    dc->start();
}

void loop()
{

}