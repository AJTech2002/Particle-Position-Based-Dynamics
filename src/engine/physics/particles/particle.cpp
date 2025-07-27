#include "tfn/grid.h"
#include "tfn/particle.h"

void tfn::Particle::assign() {
  grid->setCell(x,y,this);
}

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

bool tfn::Particle::canMove(glm::ivec2 pos)
{
    int newX = pos.x;
    int newY = pos.y;

    if (!grid->inBounds(newX, newY))
    {
        return false; // Out of bounds
    }

    Particle *oldNeighbor = grid->getOldParticle(newX, newY); 
    Particle *newNeighbor = grid->getParticle(newX, newY);
    return oldNeighbor == nullptr && newNeighbor == nullptr; // Assuming type 0 is empty
}

glm::ivec2 tfn::Particle::nextAvailableCellAlongVelocity (glm::vec2 velocity, float dt) {
  glm::ivec2 pos = {x,y};
  glm::vec2 predPos = glm::vec2(pos.x,pos.y) + velocity * dt; 
  glm::ivec2 predPosI = glm::ivec2(glm::round(predPos.x), glm::round(predPos.y));

  // check if it can move in velocity dir
  glm::ivec2 checkBuffer[500];
  int size = math::getLinePixels(
      pos,
      predPosI,
      checkBuffer
  );  
  int movedBlocks = 0;
  for (int i = 1; i < size; i++) {
    glm::ivec2& testPos = checkBuffer[i];
    if (!canMove(testPos)) {
      break;
    }

    movedBlocks = i;
  }
 
  int newX = checkBuffer[movedBlocks].x;
  int newY = checkBuffer[movedBlocks].y;

  //  tfn::Debug::drawLine(
  //   glm::vec2(pos.x, pos.y),
  //   glm::vec2(newX, newY),
  //   glm::vec3(0.0f, 1.0f, 0.0f)
  // );

  return {newX, newY};
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

void tfn::Particle::simulate(float dt)
{
  hasUpdated = true; // Mark this particle as updated
}
