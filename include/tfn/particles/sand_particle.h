#pragma once
#include "tfn/particle.h"

namespace tfn::particles
{
  class SandParticle : public Particle
  {
  public:
    SandParticle(unsigned int x, unsigned int y, int type) : Particle(x, y, type) {}
      ~SandParticle() override;  // ✅ Add this line

    void simulate(float dt) override;
    bool canMove(Direction dir) override;
    bool canMove(glm::ivec2 pos) override;
  };
}
