#pragma once 
#ifndef PARTICLE_UTILS_H
#define PARTICLE_UTILS_H
#include "glm/glm.hpp"
#include "tfn/grid.h"
#include "tfn/particle.h"
#include <vector>

static std::vector<tfn::Particle*> particlesAlongPath (tfn::ParticleGrid* grid, glm::ivec2 start, glm::ivec2 end) {
  glm::ivec2 predPosI = glm::ivec2(glm::round(end.x), glm::round(end.y));

  // check if it can move in velocity dir
  glm::ivec2 checkBuffer[1000];
  int size = math::getLinePixels(
      start,
      predPosI,
      checkBuffer
  );  

  std::vector<tfn::Particle*> particles;

  for (int i = 0; i < glm::min(1000, size); i++) {
    glm::ivec2& testPos = checkBuffer[i];
    if (!grid->inBounds(testPos.x, testPos.y)) {
      break;
    }
    
    tfn::Particle* particle = grid->getParticle(testPos.x, testPos.y);
    if (particle != nullptr) {
      particles.push_back(particle);
    } 
  }

  return particles;
}
#endif // PARTICLE_UTILS_H
