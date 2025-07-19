#pragma once
#include "tfn/particle.h"
#include "engine/physics.h"
namespace tfn::particles
{
  class SolidParticle : public Particle
  {
  public:
    SolidParticle(unsigned int x, unsigned int y, int type) : Particle(x, y, type) {}

    //PHYSICS
    physics::SBody* body;
    glm::vec2 force;
  };
}
