#include "tfn/grid.h"
#include <iostream>

using namespace tfn::particles;
SandParticle::~SandParticle() = default;


tfn::Direction getMoveableSandDirection(SandParticle *particle)
{
  if (particle->canMove(tfn::Direction::DOWN))
  {
    return tfn::Direction::DOWN;
  }
  else
  {
    bool canMoveLeftD = particle->canMove(tfn::Direction::DOWN_LEFT);
    bool canMoveRightD = particle->canMove(tfn::Direction::DOWN_RIGHT);

    if (canMoveLeftD && canMoveRightD)
    {
      // Randomly choose to move left or right
      return (rand() % 2 == 0) ? tfn::Direction::DOWN_LEFT : tfn::Direction::DOWN_RIGHT;
    }
    else if (canMoveLeftD)
    {
      return tfn::Direction::DOWN_LEFT;
    }
    else if (canMoveRightD)
    {
      return tfn::Direction::DOWN_RIGHT;
    }
  }

  return tfn::Direction::NONE; // No valid move found
}

bool SandParticle::canMove(tfn::Direction dir)
{
  std::pair<int, int> offset = getDirectionOffset(dir);
  int newX = x + offset.first;
  int newY = y + offset.second;

  if (!grid->inBounds(newX, newY))
  {
    return false; // Out of bounds
  }

  Particle *oldNeighbor = getOldNeighbour(dir);
  Particle *newNeighbor = grid->getParticle(newX, newY);

  // Can move into liquids
  return ((oldNeighbor == nullptr || oldNeighbor->type == tfn::consts::LIQUID_CELL) &&
          (newNeighbor == nullptr || newNeighbor->type == tfn::consts::LIQUID_CELL));
}

void SandParticle::simulate()
{
  // update super
  Particle::simulate();

  tfn::Direction moveDir = getMoveableSandDirection(this);
  std::pair<int, int> offset = getDirectionOffset(moveDir);

  int newX = x + offset.first;
  int newY = y + offset.second;

  // check if liquid particle is present at the new position
  if (grid->inBounds(newX, newY)) {
    Particle *liquidParticle = grid->getParticle(newX, newY);
    if (liquidParticle && liquidParticle->type == tfn::consts::LIQUID_CELL)
    {
      liquidParticle->x = x; // Update liquid particle position
      liquidParticle->y = y; // Update liquid particle position
      x = newX;
      y = newY;
      // Otherwise, treat it as blocked
    }
    else {
      x = newX;
      y = newY;
    }
  }

}

