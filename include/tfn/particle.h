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
        bool visible = true; // Flag to mark if the particle is dead
        glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
        glm::vec2 absPos = glm::vec2(0.0f, 0.0f);

        glm::ivec2 lastPosI = glm::ivec2(0, 0); // Last integer position for grid calculations
        glm::vec2 lastPos = glm::vec2(0.0f, 0.0f); // Last position for velocity calculation

        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); // Default color

        bool hasUpdated = false;
        Particle(unsigned int x, unsigned int y, int type) : x(x), y(y), type(type), absPos(x,y) {

          if (type == 0) {
            color = glm::vec3(0.0f, 0.0f, 0.0f); // Default color for empty particles
          } else if (type == 1) {
            color = glm::vec3(1.0f, 0.0f, 0.0f); // Solid particle color
          } else if (type == 2) {
            color = glm::vec3(0.0f, 1.0f, 0.0f); // Liquid particle color
          } else if (type == 3) {
            color = glm::vec3(1.0f, 1.0f, 0.0f); // Sand particle color
          }

        }
        virtual ~Particle() = default;
        virtual void simulate(float dt);
        virtual void assign(bool displayOnly = false);
        virtual bool canMove(Direction dir);
        virtual bool canMove(glm::ivec2 to);
        virtual glm::ivec2 nextAvailableCellAlongVelocity (glm::vec2 velocity, float dt); 

        Particle *getNeighbor(Direction dir) const;
        Particle *getOldNeighbour(Direction dir) const;
    };

}
