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

  // Can move into liquids
  return oldNeighbor == nullptr || oldNeighbor->type == tfn::consts::LIQUID_CELL;
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
      // Try to move the liquid left or right
      bool moved = false;
      std::pair<int, int> left = getDirectionOffset(tfn::Direction::LEFT);
      std::pair<int, int> right = getDirectionOffset(tfn::Direction::RIGHT);

      if (grid->inBounds(x + left.first, y + left.second) &&
          grid->getParticle(x + left.first, y + left.second) == nullptr)
      {
        liquidParticle->x = x + left.first;
        liquidParticle->y = y + left.second;
        moved = true;
      }
      else if (grid->inBounds(x + right.first, y + right.second) &&
              grid->getParticle(x + right.first, y + right.second) == nullptr)
      {
        liquidParticle->x = x + right.first;
        liquidParticle->y = y + right.second;
        moved = true;
      }

      // Only swap if it moved
      if (moved) {
        x = newX;
        y = newY;
      }

      // Otherwise, treat it as blocked
    }
    else {
      x = newX;
      y = newY;
    }
  }

}

