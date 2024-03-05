#ifndef __SIMPLELINE_H__
#define __SIMPLELINE_H__

#include "Generator.h"
#include "FastLEDConfig.h"

class SimpleLine : public Generator
{
public:
    SimpleLine();

protected:
    bool GenerateFrame(generatorParams params) override;

private:
    int _currentColumn = -1;
    int _currentPalleteIndex = -1;
    timestamp_t lastMovement = 0;
    CRGBPalette16 currentPalette;
};

#endif // __SIMPLELINE_H__