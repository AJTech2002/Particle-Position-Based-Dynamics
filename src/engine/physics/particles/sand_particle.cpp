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

glm::ivec2 SandParticle::nextAvailableCellAlongVelocity (glm::vec2 velocity, float dt) {
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
    Particle *newNeighbor = grid->getParticle(checkBuffer[i].x, checkBuffer[i].y);
    Particle *oldNeighbor = grid->getOldParticle(checkBuffer[i].x, checkBuffer[i].y); 

    if (newNeighbor && newNeighbor->type == tfn::consts::LIQUID_CELL)
    {
      if (oldNeighbor && oldNeighbor->type == tfn::consts::LIQUID_CELL)
      {
        // If both old and new neighbors are liquid particles, continue
        return {newNeighbor->x, newNeighbor->y};
      }
      // If a liquid particle is found, return its position
      return {newNeighbor->x, newNeighbor->y};
    }

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

glm::vec2 maxVelocity (2500.0, 2500.0);

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

  if (canMove(testPos))
  // check if liquid particle is present at the new position
  if (grid->inBounds(newX, newY)) {
    Particle *liquidParticle = grid->getParticle(newX, newY);
    if (liquidParticle && liquidParticle->type == tfn::consts::LIQUID_CELL)
    {
      /*liquidParticle->x = x; // Update liquid particle position*/
      /*liquidParticle->y = y; // Update liquid particle position*/
      /*x = newX;*/
      /*y = newY;*/
      /*// Otherwise, treat it as blocked*/
      /**/
      /*if (liquidParticle->hasUpdated) */
      /*  liquidParticle->assign();*/
    }
    else {
      x = newX;
      y = newY;
    }
  }

}

