#include "tfn/grid.h"
#include "tfn/particle.h"

bool tfn::Particle::canMove(tfn::Direction dir)
{
    std::pair<int, int> offset = getDirectionOffset(dir);
    int newX = x + offset.first;
    int newY = y + offset.second;

    if (!grid->inBounds(newX, newY))
    {
        return false; // Out of bounds
    }

    Particle *neighbor = getNeighbor(dir);
    Particle *oldNeighbor = getOldNeighbour(dir);
    return oldNeighbor == nullptr && neighbor == nullptr; // Assuming type 0 is empty
}

tfn::Particle *tfn::Particle::getNeighbor (tfn::Direction dir) const
{
    switch (dir)
    {
      case UP:
          return grid->getParticle(x, y + 1);
      case DOWN:
          return grid->getParticle(x, y - 1);
      case LEFT:
          return grid->getParticle(x - 1, y);
      case RIGHT:
          return grid->getParticle(x + 1, y);
      case UP_LEFT:
          return grid->getParticle(x - 1, y + 1);
      case UP_RIGHT:
          return grid->getParticle(x + 1, y + 1);
      case DOWN_LEFT:
          return grid->getParticle(x - 1, y - 1);
      case DOWN_RIGHT:
          return grid->getParticle(x + 1, y - 1);
    }
    return nullptr; // Invalid direction
}

tfn::Particle *tfn::Particle::getOldNeighbour (tfn::Direction dir) const
{
    switch (dir)
    {
      case UP:
          return grid->getOldParticle(x, y + 1);
      case DOWN:
          return grid->getOldParticle(x, y - 1);
      case LEFT:
          return grid->getOldParticle(x - 1, y);
      case RIGHT:
          return grid->getOldParticle(x + 1, y);
      case UP_LEFT:
          return grid->getOldParticle(x - 1, y + 1);
      case UP_RIGHT:
          return grid->getOldParticle(x + 1, y + 1);
      case DOWN_LEFT:
          return grid->getOldParticle(x - 1, y - 1);
      case DOWN_RIGHT:
          return grid->getOldParticle(x + 1, y - 1);
    }
    return nullptr; // Invalid direction
}

void tfn::Particle::simulate()
{
  hasUpdated = true; // Mark this particle as updated
}
