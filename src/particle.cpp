#include "tfn/grid.h"
#include "tfn/particle.h"

std::pair<int, int> getDirectionOffset(tfn::Direction dir)
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
    }
    return {0, 0}; // Invalid direction
}

bool tfn::Particle::canMove(tfn::Direction dir)
{
    std::pair<int, int> offset = getDirectionOffset(dir);
    int newX = x + offset.first;
    int newY = y + offset.second;

    if (!grid->inBounds(newX, newY))
    {
        return false; // Out of bounds
    }

    Particle *neighbor = getNeighbor(dir);
    return neighbor == nullptr || neighbor->type == 0; // Assuming type 0 is empty
}

void updateSand(tfn::ParticleGrid *grid, tfn::Particle &particle)
{
    if (particle.canMove(tfn::Direction::DOWN))
    {
        particle.y -= 1; // Move down
    }
    else
    {
        bool canMoveLeft = particle.canMove(tfn::Direction::DOWN_LEFT);
        bool canMoveRight = particle.canMove(tfn::Direction::DOWN_RIGHT);

        if (canMoveLeft && canMoveRight)
        {
            // Randomly choose to move left or right
            if (rand() % 2 == 0)
            {
                particle.x -= 1; // Move left
            }
            else
            {
                particle.x += 1; // Move right
            }

            particle.y -= 1; // Move down
        }
        else if (canMoveLeft)
        {
            particle.x -= 1; // Move left
            particle.y -= 1; // Move down
        }
        else if (canMoveRight)
        {
            particle.x += 1; // Move right
            particle.y -= 1; // Move down
        }
    }
}

void updateLiquid(tfn::ParticleGrid *grid, tfn::Particle &particle)
{
    if (particle.canMove(tfn::Direction::DOWN))
    {
        particle.y -= 1; // Move down
    }
    else
    {
        bool canMoveLeftD = particle.canMove(tfn::Direction::DOWN_LEFT);
        bool canMoveRightD = particle.canMove(tfn::Direction::DOWN_RIGHT);

        if (canMoveLeftD && canMoveRightD)
        {
            // Randomly choose to move left or right
            if (rand() % 2 == 0)
            {
                particle.x -= 1; // Move left
            }
            else
            {
                particle.x += 1; // Move right
            }

            particle.y -= 1; // Move down
        }
        else if (canMoveLeftD)
        {
            particle.x -= 1; // Move left
            particle.y -= 1; // Move down
        }
        else if (canMoveRightD)
        {
            particle.x += 1; // Move right
            particle.y -= 1; // Move down
        }
        else {
            bool canMoveLeft = particle.canMove(tfn::Direction::LEFT);
            bool canMoveRight = particle.canMove(tfn::Direction::RIGHT);

            if (canMoveLeft && canMoveRight)
            {
                // Randomly choose to move left or right
                if (rand() % 2 == 0)
                {
                    particle.x -= 1; // Move left
                }
                else
                {
                    particle.x += 1; // Move right
                }
            }
            else if (canMoveLeft)
            {
                particle.x -= 1; // Move left
            }
            else if (canMoveRight)
            {
                particle.x += 1; // Move right
            }
        }
    }
}

tfn::Particle *tfn::Particle::getNeighbor(tfn::Direction dir) const
{
    switch (dir)
    {
    case UP:
        return grid->getParticle(x, y + 1);
    case DOWN:
        return grid->getParticle(x, y - 1);
    case LEFT:
        return grid->getParticle(x - 1, y);
    case RIGHT:
        return grid->getParticle(x + 1, y);
    case UP_LEFT:
        return grid->getParticle(x - 1, y + 1);
    case UP_RIGHT:
        return grid->getParticle(x + 1, y + 1);
    case DOWN_LEFT:
        return grid->getParticle(x - 1, y - 1);
    case DOWN_RIGHT:
        return grid->getParticle(x + 1, y - 1);
    }
    return nullptr; // Invalid direction
}

void tfn::Particle::simulate()
{
    switch (type)
    {
    case tfn::consts::SOLID_CELL:
        // Solid particles do not move
        break;
    case tfn::consts::LIQUID_CELL:
        // Simulate liquid behavior
        updateLiquid(grid, *this);
        break;
    case tfn::consts::SAND_CELL:
        updateSand(grid, *this);
        break;
    default:
        // Handle unknown particle type
        break;
    }
}