#pragma once
#include <stdexcept>
#include "tfn/particle.h"
#include "tfn/constants.h"
#include <vector>
#include <array>
#include <unordered_map>

// Particle types
#include "tfn/particles/sand_particle.h"
#include "tfn/particles/liquid_particle.h"
#include "tfn/particles/solid_particle.h"

namespace tfn
{
  struct Cell
  {
    Particle *lastParticle = nullptr; // Pointer to the last particle added to this cell
    Particle *particle = nullptr;     // Pointer to the particle in this cell, if any

    // Per cell values
    float divergence = 0.0f; // Divergence value for the cell
  };

  struct DisplayCell
  {
    // int type;
    float r, g, b, a; // Color for display purposes
  };

  class ParticleGrid
  {
  private:
    Cell *cells;
    std::vector<Particle *> particles;
    std::unordered_map<int, std::vector<Particle *>> particleMap; // Map for quick access to particles by their position

  public:
    unsigned int width;
    unsigned int height;
    DisplayCell *display;

    ParticleGrid(unsigned int w, unsigned int h) : width(w), height(h)
    {
      cells = new Cell[width * height];
      display = new DisplayCell[width * height];

      // Reserve space to prevent vector reallocation which would invalidate pointers
      particles.reserve(w * h); // Reserve space for reasonable number of particles

      for (unsigned int i = 0; i < width * height; ++i)
      {
        display[i].r = 0.0f;
        display[i].g = 0.0f;
        display[i].b = 0.0f;
        display[i].a = 0.0f; // Default alpha value for display cells
      }
    }

    ~ParticleGrid()
    {
      delete[] cells;
      delete[] display;
      // Particles will be automatically cleaned up by the vector destructor
    }

    Cell *getCells()
    {
      return cells;
    }

    Cell *getCell(unsigned int x, unsigned int y)
    {
      if (x < width && y < height)
      {
        return &getCells()[y * width + x];
      }
      throw std::out_of_range("Grid coordinates out of bounds (getCell)");
    }

    void setDisplay(int x, int y, Particle *particle)
    {
      if (x >= 0 && y >= 0 && x < width && y < height)
      {
        // display[y * width + x].type = particle->type; // Update display cell type
        display[y * width + x].r = particle->color.r;
        display[y * width + x].g = particle->color.g;
        display[y * width + x].b = particle->color.b;
        display[y * width + x].a = 0.0f; // Default alpha
      }
      else
      {
        /*throw std::out_of_range("Grid coordinates out of bounds (setCell)");*/
      }
    }

    void setDisplay(int x, int y,  glm::vec3 col)
    {
      if (x >= 0 && y >= 0 && x < width && y < height)
      {
        // display[y * width + x].type = particle->type; // Update display cell type
        display[y * width + x].r = col.r;
        display[y * width + x].g = col.g;
        display[y * width + x].b = col.b;
        display[y * width + x].a = 0.0f; // Default alpha
      }
      else
      {
        /*throw std::out_of_range("Grid coordinates out of bounds (setCell)");*/
      }
    }

    void setCell(int x, int y, Particle *particle)
    {
      if (x >= 0 && y >= 0 && x < width && y < height)
      {
        if (getCell(x, y)->particle == nullptr && particle->visible)
        {
          getCell(x, y)->particle = particle; // Set the particle pointer in the cell
          setDisplay(x, y, particle);         // Update display cell type
        }

        particleMap[x + y * width].push_back(particle); // Add particle to the map for quick access
      }
      else
      {
        /*throw std::out_of_range("Grid coordinates out of bounds (setCell)");*/
      }
    }

    const std::vector<Particle *> &getParticles()
    {
      return particles;
    }

    std::array<const Cell*, 8> getNeighbouringCells(int x, int y) {
      std::array<const Cell*, 8> neighbours{};
      int index = 0;
      for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
          if (dx == 0 && dy == 0) continue;
          int nx = x + dx;
          int ny = y + dy;
          if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
            neighbours[index++] = getCell(nx, ny);
          }
        }
      }
      return neighbours;
    }

    std::vector<Particle *> &getParticlesAt(int x, int y)
    {
      if (x >= 0 && y >= 0 && x < width && y < height)
      {
        // check if it exists in the map, if not return empty vector
        if (particleMap.find(x + y * width) != particleMap.end())
        {

          return particleMap[x + y * width];
        }
        else
        {
          particleMap[x + y * width] = std::vector<Particle *>();
          return particleMap[x + y * width]; // Return an empty vector if no particles are
        }
      }
      throw std::out_of_range("Grid coordinates out of bounds (getParticlesAt)");
    }

    Particle* getParticle(int x, int y)
    {
      if (x >= 0 && y >= 0 && x < width && y < height)
      {
        int index = cellIndex(x, y);
        return getCells()[index].particle;
      }
      return nullptr; // No particle found at the given coordinates
    }

    glm::vec2 getVelocity (int x, int y)
    {
      Particle* p = getParticle(x, y);
      if (p != nullptr)
      {
        return p->velocity;
      }

      return glm::vec2(0.0f, 0.0f); // Return zero velocity if no particle is found
    }


    Particle *getOldParticle(unsigned int x, unsigned int y)
    {
      if (x < width && y < height)
      {
        int index = cellIndex(x, y);
        return getCells()[index].lastParticle;
      }
      return nullptr; // No particle found at the given coordinates
    }

    const int cellIndex(unsigned int x, unsigned int y) const
    {
      return y * width + x;
    }

    Particle *createParticle(int x, int y, int type)
    {
      switch (type)
      {
      case tfn::consts::EMPTY_CELL:
        return new Particle(x, y, type);
      case tfn::consts::SOLID_CELL:
        return new tfn::particles::SolidParticle(x, y, type);
      case tfn::consts::LIQUID_CELL:
        return new tfn::particles::LiquidParticle(x, y, type);
      case tfn::consts::SAND_CELL:
        return new tfn::particles::SandParticle(x, y, type);
      default:
        throw std::invalid_argument("Unknown particle type");
      }
    }

    Particle *addParticle(unsigned int x, unsigned int y, int type)
    {
      // check if particle is already present at cell indexs
      if (getParticle(x, y) != nullptr || type == tfn::consts::EMPTY_CELL)
      // If the cell already has a particle or is empty, do not add a new one
      {
        return nullptr;
      }

      if (x < width && y < height)
      {
        // particles.emplace_back(x, y, type);
        // Particle &newParticle = particles.back();

        Particle *newParticle = createParticle(x, y, type);
        particles.push_back(newParticle); // Add the new particle to the vector
        newParticle->grid = this;

        getCell(x, y)->particle = newParticle; // Set the particle pointer in the cell
        return newParticle;
      }

      throw std::out_of_range("Particle coordinates out of bounds");
    }

    bool inBounds(int x, int y) const
    {
      return (x < width && y < height);
    }

    void clear()
    {
      for (unsigned int i = 0; i < width * height; ++i)
      {
        display[i].r = 0.0f;
        display[i].g = 0.0f;
        display[i].b = 0.0f;
        display[i].a = 0.0f;

        if (cells[i].particle != nullptr)
          cells[i].particle->hasUpdated = false;

        cells[i].lastParticle = cells[i].particle; // Store the last particle before clearing
        cells[i].particle = nullptr;               // Clear the particle pointer in the cell
      }

      particleMap.clear(); // Clear the particle map
    }

    void update(float dt, bool simulate = false);

    void debug();

    void cleanup()
    {
      for (auto particle : particles)
      {
        delete particle; // Clean up each particle
      }
      particles.clear(); // Clear the vector of particles
    }
  };

}
