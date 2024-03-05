#ifndef __CHUNKER_H__
#define __CHUNKER_H__

#include "FastLEDConfig.h"
#include "Generator.h"

class Chunker : public Generator
{
  public:
    Chunker();

  protected:
    bool GenerateFrame(generatorParams params) override;

  private:
    int _currentPalleteOffset = 0;
    CRGBPalette16 currentPalette;
    timestamp_t _lastTimestamp = 0;

};

#endif // __CHUNKER_H__