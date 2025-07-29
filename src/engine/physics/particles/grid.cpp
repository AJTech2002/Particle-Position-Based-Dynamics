#include "tfn/grid.h"
#include "tfn/particle.h"
#include <random>

static std::mt19937 rng(std::random_device{}());

void tfn::ParticleGrid::update(float dt, bool simulate) {
  this->clear();

  // TODO: Implement a more sophisticated update logic
  // Randomize so that particles don't create a predictable pattern
  std::shuffle(particles.begin(), particles.end(), rng);

  for (auto& particle : particles) {

    if (particle->dead) {
      continue; // Skip dead particles
    }

    if (simulate)  {
      int oldX = particle->x;
      int oldY = particle->y;
      particle->simulate(dt);
    }

    if (particle->hasUpdated || !simulate) {
      if (inBounds(particle->x, particle->y) &&
        getCell(particle->x, particle->y)->particle == nullptr) {
        setCell(particle->x, particle->y, particle);
      }
      else {
        particle->dead = true;
      }
    }
  }
}
