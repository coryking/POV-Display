#include "SimpleLine.h"
#include "esp_log.h"

static const char *TAG = "SimpleLine";

bool SimpleLine::GenerateFrame(generatorParams params)
{
    CRGB *fb = params.framebuffer->getBuffer();
    if (params.frame_time - lastMovement > 1000 * 1000)
    {
        _currentColumn = (_currentColumn + 1) % params.framebuffer->getCols();
        lastMovement = params.frame_time;
    }
    _currentPalleteIndex = (_currentPalleteIndex + 1) % 255;
   // CRGB color = ColorFromPalette(currentPalette, _currentPalleteIndex, 255, LINEARBLEND);
    for (int row = 0; row < params.framebuffer->getRows(); row++)
    {
        int index = row * params.framebuffer->getCols() + _currentColumn;

        fb[index] = CRGB::CadetBlue;
    }
    return true;
}

SimpleLine::SimpleLine() : Generator()
{
    currentPalette = PartyColors_p;
}
