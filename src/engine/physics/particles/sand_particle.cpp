#include "engine/math.h"
#include "tfn/grid.h"
#include "engine/debug.h"
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

glm::vec2 maxVelocity (2500.0, 2500.0);

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

bool SandParticle::canMove(glm::ivec2 newPos) {
  int newX = newPos.x;
  int newY = newPos.y;

  if (!grid->inBounds(newX, newY))
  {
    return false; // Out of bounds
  }

  Particle *oldNeighbor = grid->getOldParticle(newX, newY); 
  Particle *newNeighbor = grid->getParticle(newX, newY);

  // Can move into liquids
  return ((oldNeighbor == nullptr || oldNeighbor->type == tfn::consts::LIQUID_CELL) &&
      (newNeighbor == nullptr || newNeighbor->type == tfn::consts::LIQUID_CELL));
}


float scatterDistance = 100.0;

void SandParticle::simulate(float dt)
{
  // update super
  Particle::simulate(dt);
  
  glm::ivec2 curPos = {x,y};

  // update velocity with gravity
  velocity.y += -981.0f * dt;
  velocity = glm::clamp(velocity, -maxVelocity, maxVelocity);

  glm::ivec2 testPos = {x,y};

  testPos = nextAvailableCellAlongVelocity(velocity, dt);
 
  // No more room below, modify vel x and damp y
  if (testPos == curPos) {
    // Spread sideways when blocked
    int dir = (math::randMultiplier() > 0 ? 1 : -1);
    if (canMove({x + dir, y - 1})) {
      velocity.x += dt * 600.0f * dir;
      velocity.y *= 0.95f;
    } else if (canMove({x - dir, y - 1})) {
      velocity.x += dt * -600.0f * dir;
      velocity.y *= 0.95f;
    } else {
      // Dampen when stuck
      velocity *= 0.5f;
    }
  }         


  testPos = nextAvailableCellAlongVelocity( velocity, dt);

  int newX = testPos.x;
  int newY = testPos.y;

  if (testPos != curPos)
    velocity.x *= 0.95; 

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

