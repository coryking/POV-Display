#include "Chunker.h"

static const char *TAG = "SimpleLine";

bool Chunker::GenerateFrame(generatorParams params)
{
    CRGB *fb = params.framebuffer->getBuffer();
    timestamp_t tdiff = params.frame_time - _lastTimestamp;
    if (tdiff > 500 * 1000)
    {
        _currentPalleteOffset = (_currentPalleteOffset + 1) % 255;
        _lastTimestamp = params.frame_time;
    }
    for (int col = 0; col < params.framebuffer->getCols(); col++)
    {

        int pidx = (_currentPalleteOffset + map(col, 0, params.framebuffer->getCols() - 1, 0, 255)) % 255;
        CRGB color = ColorFromPalette(currentPalette, pidx, 255, LINEARBLEND);
        //ESP_LOGV(TAG, "pidx: %d, r: %d, g: %d, b: %d", pidx, color.r, color.g, color.b);

        for (int row=0; row < params.framebuffer->getRows(); row++)
        {
            int idx = col + row * params.framebuffer->getCols();
            fb[idx] = color;
        }
    }
    ESP_LOGD(TAG, "cpo: %d, lt: %llu, ltms: %llu, tdiff: %llu", _currentPalleteOffset, _lastTimestamp,
             _lastTimestamp / 1000, tdiff);
    return true;
}

Chunker::Chunker()
{
    currentPalette = PartyColors_p;
}
