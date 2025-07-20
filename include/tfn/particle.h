#pragma once
#include <utility>
#include "glm/glm.hpp"

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
        DOWN_RIGHT,
        NONE
    };

    inline std::pair<int, int> getDirectionOffset(tfn::Direction dir)
    {
        switch (dir)
        {
        case tfn::Direction::UP:
            return {0, 1};
        case tfn::Direction::DOWN:
            return {0, -1};
        case tfn::Direction::LEFT:
            return {-1, 0};
        case tfn::Direction::RIGHT:
            return {1, 0};
        case tfn::Direction::UP_LEFT:
            return {-1, 1};
        case tfn::Direction::UP_RIGHT:
            return {1, 1};
        case tfn::Direction::DOWN_LEFT:
            return {-1, -1};
        case tfn::Direction::DOWN_RIGHT:
            return {1, -1};
        case tfn::Direction::NONE:
            return {0, 0}; // No movement
        }
        return {0, 0}; // Invalid direction
    }

    class Particle
    {
    public:
        int x;
        int y;
        int type;
        ParticleGrid *grid;
        glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
        glm::vec2 absPos = glm::vec2(0.0f, 0.0f);

        bool hasUpdated = false;
        Particle(unsigned int x, unsigned int y, int type) : x(x), y(y), type(type), absPos(x,y) {}
        virtual ~Particle() = default;
        virtual void simulate();
        virtual void assign();
        virtual bool canMove(Direction dir);
        
        Particle *getNeighbor(Direction dir) const;
        Particle *getOldNeighbour(Direction dir) const;
    };

}
