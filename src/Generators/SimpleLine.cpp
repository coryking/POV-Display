#include <FastLED.h>
#include "SimpleLine.h"
bool SimpleLine::GenerateFrame(generatorParams params)
{

    CRGB *fb = params.framebuffer->getBuffer();
    _currentRow = (_currentRow + 1) % params.framebuffer->getRows();
    _currentPalleteIndex = (_currentPalleteIndex + 1) & 255;
    CRGB color = ColorFromPalette(currentPalette, _currentPalleteIndex, 255, NOBLEND);
    for (int col = 0; params.framebuffer->getCols(); col++)
    {
        int index = _currentRow * params.framebuffer->getCols() + col;

        fb[index] = color;
    }
    return true;
}

SimpleLine::SimpleLine() : Generator()
{
    currentPalette = PartyColors_p;
}
