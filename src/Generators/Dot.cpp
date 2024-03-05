#include "Dot.h"

bool Dot::GenerateFrame(generatorParams params)
{
    params.framebuffer->getBuffer()[0] = CRGB::Red;
    params.framebuffer->getBuffer()[9] = CRGB::Green;
    params.framebuffer->getBuffer()[29] = CRGB::Blue;
    return true;
}
