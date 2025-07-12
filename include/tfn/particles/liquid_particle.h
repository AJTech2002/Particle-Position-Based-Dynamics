#pragma once
#include "tfn/particle.h"

namespace tfn::particles
{
  class LiquidParticle : public Particle
  {
  public:
    LiquidParticle(unsigned int x, unsigned int y, int type) : Particle(x, y, type) {}

    void simulate() override
    {
      Particle::simulate();
      // Implement liquid particle behavior here
      // For example, move down if possible
      if (canMove(Direction::DOWN))
      {
        y -= 1; // Move down
      }
      else
      {
        bool canMoveLeftD = canMove(tfn::Direction::DOWN_LEFT);
        bool canMoveRightD = canMove(tfn::Direction::DOWN_RIGHT);

        if (canMoveLeftD && canMoveRightD)
        {
          // Randomly choose to move left or right
          if (rand() % 2 == 0) x -= 1; // Move left
          else x += 1; // Move right
          y -= 1; // Move down
        }
        else if (canMoveLeftD)
        {
          x -= 1; // Move left
          y -= 1; // Move down
        }
        else if (canMoveRightD)
        {
          x += 1; // Move right
          y -= 1; // Move down
        }
        else
        {
          bool canMoveLeft = canMove(tfn::Direction::LEFT);
          bool canMoveRight = canMove(tfn::Direction::RIGHT);

          if (canMoveLeft && canMoveRight)
          {
            // Randomly choose to move left or right
            if (rand() % 2 == 0) x -= 1; // Move left
            else x += 1; // Move right
          }
          else if (canMoveLeft) x -= 1; // Move left
          else if (canMoveRight) x += 1; // Move right
        }
      }
    }
  };
}