#pragma once
#include <utility>
#include "glm/glm.hpp"
#include "engine/math.h"

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

        glm::vec3 restColor = glm::vec3(1.0f, 1.0f, 1.0f); // Default color for particles
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f); // Default color

        float density = 0.0f; // Density for the particle
        float friction = 0.0f; // Friction for the particle
        float bounce = 0.0f; // Bounce factor for the particle

        bool hasUpdated = false;
        Particle(unsigned int x, unsigned int y, int type) : x(x), y(y), type(type), absPos(x,y) {

          if (type == 0) {
            restColor = glm::vec3(0.0f, 0.0f, 0.0f); // Default color for empty particles
            density = 0.0f; // Default density for empty particles
          } else if (type == 1) {
            /*restColor = glm::vec3(1.0f, 0.0f, 0.0f); // Solid particle color*/
            restColor = glm::vec3(1.0, 1.0, 1.0); // Solid particle color (dark red)
            density = 1.0f; // Default density for solid particles
          } else if (type == 2) {
            restColor = glm::vec3(0.2f, 0.2f, 0.9f); // Liquid particle color
            density = 90.0f; // Default density for liquid particles
            friction = 0.01f; // Default friction for liquid particles 
          } else if (type == 3) {
            restColor = glm::vec3(0.2f, 0.2f, 0.2f); // Sand particle color
          restColor *= 0.8f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.4f; // Randomly darken the color
            density = 100.0f;
            friction = 10.0f; // Default friction for sand particles
          }

          // add some variation in darkness
          color = restColor; // Initialize color to restColor

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
