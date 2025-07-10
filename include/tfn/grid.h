#pragma once
#include <stdexcept>
#include "tfn/particle.h"
#include "tfn/constants.h"
#include <vector>
#include <unordered_map>

namespace tfn
{
    struct Cell
    {
        Particle *particle = nullptr; // Pointer to the particle in this cell, if any
    };

    struct DisplayCell
    {
        int type;
    };

    class ParticleGrid
    {
    private:
        Cell *cells;
        std::vector<Particle> particles;

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
                display[i].type = 0; // Initialize display cells to type 0
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

        Cell &getCell(unsigned int x, unsigned int y)
        {
            if (x < width && y < height)
            {
                return getCells()[y * width + x];
            }
            throw std::out_of_range("Grid coordinates out of bounds");
        }

        void setCell(unsigned int x, unsigned int y, Particle &particle)
        {
            if (x < width && y < height)
            {
                getCell(x, y).particle = &particle; // Set the particle pointer in the cell
                display[y * width + x].type = particle.type; // Update display cell type
            }
            else
            {
                throw std::out_of_range("Grid coordinates out of bounds");
            }
        }

        const std::vector<Particle> &getParticles()
        {
            return particles;
        }

        Particle *getParticle(unsigned int x, unsigned int y)
        {
            if (x < width && y < height)
            {
                int index = cellIndex(x, y);
                return getCells()[index].particle;
            }
            return nullptr; // No particle found at the given coordinates
        }

        const int cellIndex(unsigned int x, unsigned int y) const
        {
            if (x < width && y < height)
            {
                return y * width + x;
            }
            throw std::out_of_range("Grid coordinates out of bounds");
        }

        void addParticle(unsigned int x, unsigned int y, int type)
        {
            // check if particle is already present at cell indexs
            if (getParticle(x, y) != nullptr)
            {
                return;
            }

            if (x < width && y < height)
            {
                particles.emplace_back(x, y, type);
                Particle &newParticle = particles.back();
                newParticle.grid = this;
                getCell(x, y).particle = &newParticle; // Set the particle pointer in the cell
                return;
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
                display[i].type = tfn::consts::EMPTY_CELL;
                cells[i].particle = nullptr; // Clear the particle pointer in the cell
            }
        }

        void update();
    };

}