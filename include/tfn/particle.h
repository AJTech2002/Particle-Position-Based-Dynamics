#pragma once
#include <utility>

namespace tfn
{
    class ParticleGrid;

    enum Direction
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        UP_LEFT,
        UP_RIGHT,
        DOWN_LEFT,
        DOWN_RIGHT
    };

    class Particle
    {
    public:
        int x;
        int y;
        int type;
        ParticleGrid *grid;

        Particle(unsigned int x, unsigned int y, int type) : grid(grid), x(x), y(y), type(type) {}

        void simulate();
        Particle *getNeighbor(Direction dir) const;
        bool canMove(Direction dir);
    };

}