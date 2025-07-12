#pragma once
#include "tfn/particle.h"

namespace tfn::particles
{
  class SolidParticle : public Particle
  {
  public:
    SolidParticle(unsigned int x, unsigned int y, int type) : Particle(x, y, type) {}
  };
}