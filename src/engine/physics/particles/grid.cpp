#include "tfn/grid.h"
#include "tfn/particle.h"
#include <random>

static std::mt19937 rng(std::random_device{}());

void tfn::ParticleGrid::update(float dt, bool simulate)
{
  this->clear();

  // TODO: Implement a more sophisticated update logic
  // Randomize so that particles don't create a predictable pattern
  std::shuffle(particles.begin(), particles.end(), rng);

  for (auto &particle : particles)
  {
    if (simulate)
    {
      int oldX = particle->x;
      int oldY = particle->y;
      particle->simulate(dt);
    }

    if (particle->hasUpdated || !simulate)
    {
      if (inBounds(particle->x, particle->y))
      {
        setCell(particle->x, particle->y, particle);
      }

      particle->visible = true;
    }
  }

  // Compute divergency / pressure (TODO: Perhaps do this with particle update)

  for (int x = 1; x < width - 1; ++x)
  {
    for (int y = 1; y < height - 1; ++y)
    {
      glm::vec2 vR = getVelocity(x + 1, y);
      glm::vec2 vL = getVelocity(x - 1, y);
      glm::vec2 vU = getVelocity(x, y + 1);
      glm::vec2 vD = getVelocity(x, y - 1);

      float div = (vR.x - vL.x + vU.y - vD.y) * 0.5f;
      getCell(x, y)->divergence = div;
    }
  }
}

void tfn::ParticleGrid::debug()
{
  for (int x = 0; x < width; ++x)
  {
    for (int y = 0; y < height; ++y)
    {
      Cell *cell = getCell(x, y);

      float absDiv = glm::abs(cell->divergence);
      glm::vec3 posDiv = glm::vec3(0.0f, 1.0f, 0.0f);
      glm::vec3 negDiv = glm::vec3(1.0f, 0.0f, 0.0f);

      setDisplay(x, y, cell->divergence >= 0 ? posDiv * absDiv / 100.0F : negDiv * absDiv / 100.0F ); // Clear display cells
    }
  }
}