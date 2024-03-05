#pragma once

#include "FastLEDConfig.h"
#include "Generator.h"

class Dot : public Generator
{
  public:
    Dot() {}

  protected:
    bool GenerateFrame(generatorParams params) override;

  private:
};
