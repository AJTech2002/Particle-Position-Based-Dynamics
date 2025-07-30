#pragma once
#include "tfn/particle.h"
#include "glm/glm.hpp"
#include "engine/math.h"

namespace tfn::particles
{
  class LiquidParticle : public Particle
  {
    public:
      LiquidParticle(unsigned int x, unsigned int y, int type) : Particle(x, y, type) {}
      void simulate(float dt) override {
        Particle::simulate(dt);
        glm::vec2 maxVelocity(5000.0, 5000.0);
        velocity.y += -981.0f * dt;
        velocity = glm::clamp(velocity, -maxVelocity, maxVelocity);

        glm::ivec2 curPos = {x, y};
        glm::ivec2 nextPos = nextAvailableCellAlongVelocity(velocity, dt);

        if (nextPos == curPos) {
          // Spread sideways when blocked
          int dir = (math::randMultiplier() > 0 ? 1 : -1);
          if (canMove({x + dir, y - 1})) {
            velocity.x += dt * 1500.0f * dir;
            velocity.y *= 0.95f;
          } else if (canMove({x - dir, y - 1})) {
            velocity.x += dt * -1500.0f * dir;
            velocity.y *= 0.95f;
          }
          else if (canMove({x + dir, y})) {
            velocity.x += dt * 1500.0f * dir;
            velocity.y *= 0.5f;
          } else if (canMove({x - dir, y})) {
            velocity.x += dt * -1500.0f * dir;
            velocity.y *= 0.5f;
          } else {
            // Dampen when stuck
            // velocity *= 0.5f;
          }
        }         

        nextPos = nextAvailableCellAlongVelocity(velocity, dt);
        x = nextPos.x;
        y = nextPos.y;

        velocity *= 0.95f; // damping
      }
  };
}
