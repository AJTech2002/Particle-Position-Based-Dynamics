#include "engine/physics.h"
#include "engine/engine.h"
#include "engine/debug.h"
#include "engine/math.h"
#include "engine/utils.h"
#include "tfn/grid.h"
#include "tfn/particles/solid_particle.h"
#include <vector>
#include <iostream>

namespace physics
{

  Solver::Solver()
  {
  }

  Solver::~Solver()
  {
    for (int i = 0; i < bodyCount; i++)
    {
      delete bodies[i];
    }
  }

  SBody *createBoxBody(glm::vec2 center, glm::vec2 size)
  {
    // Debug Body for now
    tfn::Square square = {
        .center = center,
        .size = size};

    const glm::vec2 tR = square.center + square.size * (float)0.5;
    const glm::vec2 tL = square.center + glm::vec2(-square.size.x / 2, square.size.y / 2);
    const glm::vec2 bR = square.center + glm::vec2(square.size.x / 2, -square.size.y / 2);
    const glm::vec2 bL = square.center - square.size * (float)0.5;

    SBody *pBody = new SBody();
    SBody &body = *pBody;

    body.shape.points.reserve(4);
    body.shape.points.push_back({.pos = tR});

    body.shape.points.push_back({.pos = tL});

    body.shape.points.push_back({.pos = bR});

    body.shape.points.push_back({.pos = bL});

    // shape
    body.shape.mesh.push_back(0);
    body.shape.mesh.push_back(1);
    body.shape.mesh.push_back(2);

    body.shape.mesh.push_back(3);
    body.shape.mesh.push_back(2);
    body.shape.mesh.push_back(1);

    // constraints
    Constraint aC = {.type = ConstraintType::SPRING};
    Constraint bC = {.type = ConstraintType::SPRING};
    Constraint cC = {.type = ConstraintType::SPRING};
    Constraint dC = {.type = ConstraintType::SPRING};
    Constraint eC = {.type = ConstraintType::SPRING};

    aC.constraint.spring.init(&body.shape.points[0], &body.shape.points[1]);
    bC.constraint.spring.init(&body.shape.points[0], &body.shape.points[2]);

    cC.constraint.spring.init(&body.shape.points[3], &body.shape.points[1]);
    dC.constraint.spring.init(&body.shape.points[3], &body.shape.points[2]);

    eC.constraint.spring.init(&body.shape.points[0], &body.shape.points[3]);

    body.constraints.push_back(aC);
    body.constraints.push_back(bC);
    body.constraints.push_back(cC);
    body.constraints.push_back(dC);
    body.constraints.push_back(eC);

    return pBody;
  }

  void Solver::createBodyFromPoints(std::vector<tfn::Particle *> particles)
  {
    glm::vec2 min;
    min.x = 99999;
    min.y = 99999;

    glm::vec2 max;
    max.x = 0;
    max.y = 0;

    for (const auto &p : particles)
    {
      min = glm::min(min, glm::vec2(p->x, p->y));
      max = glm::max(max, glm::vec2(p->x, p->y));
    }

    glm::vec2 center = (max + min) / (float)2;
    SBody *b = createBoxBody(center, max - min);

    // Figure out which triangles and where each particle belongs to

    for (tfn::Particle *p : particles)
    {

      glm::vec2 particlePos(p->x, p->y);
      tfn::particles::SolidParticle *solid = static_cast<tfn::particles::SolidParticle *>(p);

      for (int i = 0; i < b->shape.mesh.size(); i += 3)
      {
        int pA = b->shape.mesh[i + 0];
        int pB = b->shape.mesh[i + 1];
        int pC = b->shape.mesh[i + 2];

        glm::vec2 posA = b->shape.points[pA].pos;
        glm::vec2 posB = b->shape.points[pB].pos;
        glm::vec2 posC = b->shape.points[pC].pos;

        math::BarycentricResult res = math::barycentricFromWorld(posA, posB, posC, particlePos);

        if (res.inside)
        {
          solid->bary = res.bary;
          solid->triIndex = i / 3;
          solid->body = b;

          b->particles.push_back(solid);
          /*std::cout << "Found tri: " << solid->triIndex << std::endl; */
          break;
        }
      }
    }

    bodies[bodyCount] = b;
    bodyCount++;
  }

  void Solver::init() {

  };

  glm::vec2 maxVelocity = glm::vec2(10.0f, 105.0f); // Max velocity to prevent too fast movement

  void Solver::simulate(float dt)
  {
    int subSteps = 5;
    float subDt = dt / (float)subSteps;

    for (int step = 0; step < subSteps; step++)
    {
      for (int i = 0; i < bodyCount; i++)
      {
        SBody &b = *bodies[i];

        // 1. Apply gravity (predict positions)
        for (auto &p : b.shape.points)
        {
          p.vel += glm::vec2(0, -9.81f * 10.0f) * subDt;
          p.prevPos = p.pos;      // store for velocity update later
          p.pos += p.vel * subDt; // predicted position
        }

        // 2. Solve constraints (iterative position projection)
        int iterations = 5; // tweak for stiffness
        for (int iter = 0; iter < iterations; iter++)
        {
          for (auto &c : b.constraints)
          {
            if (c.type == ConstraintType::SPRING)
              c.constraint.spring.project(); // direct PBD projection
          }
        }

        // 3. Update velocities after projection
        for (auto &p : b.shape.points)
        {
          p.vel = (p.pos - p.prevPos) / subDt;
          /*p.pos = glm::clamp(p.pos, glm::vec2(-400, -400), glm::vec2(400, 400));*/

          if (p.pos.y < 0)
          {
            p.pos.y = 0;
            p.vel.y *= -0.75;
          }

          // Clamp velocities to support stepping through to resolve overlaps
          p.vel = glm::clamp(p.vel, -maxVelocity, maxVelocity);
        }
      }
    }
  }

  // Set particle positions and handle particle overlaps
  void Solver::propogate(float fixedDt)
  {
    std::unordered_map<int, tfn::Particle *> particleMap;
    for (int bi = 0; bi < bodyCount; bi++)
    {
      SBody *body = bodies[bi];

      for (auto &p : body->particles)
      {
        int i = p->triIndex * 3;
        int pA = body->shape.mesh[i + 0];
        int pB = body->shape.mesh[i + 1];
        int pC = body->shape.mesh[i + 2];

        glm::vec2 posA = body->shape.points[pA].pos;
        glm::vec2 posB = body->shape.points[pB].pos;
        glm::vec2 posC = body->shape.points[pC].pos;

        glm::vec2 newPos = math::worldFromBarycentric(posA, posB, posC, p->bary);
        // Calculate velocity
        p->velocity = (newPos - p->absPos) / fixedDt;

        p->lastPos = p->absPos;               // Store last position for velocity calculation
        p->lastPosI = glm::ivec2(p->x, p->y); // Store last integer position for grid calculations
        p->absPos.x = newPos.x;
        p->absPos.y = newPos.y;

        p->grid->getCell(p->x, p->y)->particle = nullptr;

        p->x = glm::round(newPos.x);
        p->y = glm::round(newPos.y);

        p->hasUpdated = true;

        // Debug
        // p->assign();
        particleMap[p->x + p->y * p->grid->width] = p; // Store particle in map for quick access
      }
    }

    // ----

    for (int bi = 0; bi < bodyCount; bi++)
    {
      SBody *body = bodies[bi];

      std::vector<tfn::Particle *> _particlesAlongPathUpdated;
      std::unordered_map<tfn::Particle *, tfn::Particle *> collidedParticles;

      for (auto &p : body->particles)
      {
        std::vector<tfn::Particle *> _particlesAlongPath = particlesAlongPath(p->grid, p->lastPosI, glm::ivec2(p->x, p->y));

        for (tfn::Particle *other : _particlesAlongPath)
        {
          if (other == p)
            continue; // Skip self
          if (other->type == tfn::consts::SOLID_CELL)
            continue; // Skip solid cells

          if (collidedParticles.find(other) != collidedParticles.end())
          {
            // Some other particle has already collided with this particle
            // Compare distance to the last position
            tfn::Particle *collidedWith = collidedParticles[other];

            if (glm::distance(glm::vec2(other->x, other->y), p->lastPos) < glm::distance(glm::vec2(other->x, other->y), collidedWith->lastPos))
            {
              // If the current particle is closer to the last position, update the collided particle
              collidedParticles[other] = p;
            }
          }
          else
          {
            _particlesAlongPathUpdated.push_back(other);
            collidedParticles[other] = p; // Store the particle that collided with this one
          }
        }
      }

      for (auto &other : _particlesAlongPathUpdated)
      {
        tfn::Particle *p = collidedParticles[other];
        glm::ivec2 start = glm::ivec2(p->x, p->y);
        glm::ivec2 offset = glm::ivec2(p->x - p->lastPosI.x, p->y - p->lastPosI.y);
        glm::ivec2 endCheck = start + offset * 100;

        // tfn::Debug::getInstance().drawLine(
        //     glm::vec2(start.x, start.y),
        //     glm::vec2(endCheck.x, endCheck.y),
        //     glm::vec3(0.0f, 1.0f, 0.0f),
        //     0.1f);

        tfn::Debug::getInstance().drawLine(
            glm::vec2(other->x, other->y),
            glm::vec2(endCheck.x, endCheck.y),
            glm::vec3(1.0f, 1.0f, 0.0f),
            0.1f);

        tfn::Debug::getInstance().drawSquare(
            glm::vec2(start.x, start.y),
            glm::vec2(0.5f, 0.5f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            2.2f);

        tfn::Debug::getInstance().drawSquare(
            glm::vec2(other->x, other->y),
            glm::vec2(0.5f, 0.5f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            2.2f);

        glm::ivec2 checkBuffer[1000];
        int size = math::getLinePixels(
            start,
            endCheck,
            checkBuffer);

        int moveTo = 0;
        for (int i = 1; i < glm::min(1000, size); i++)
        {
          glm::ivec2 &testPos = checkBuffer[i];
          if (!p->grid->inBounds(testPos.x, testPos.y))
          {
            break;
          }

          if (particleMap.find(testPos.x + testPos.y * p->grid->width) == particleMap.end())
          {
            // If there is a particle at this position, stop
            moveTo = i;

            tfn::Debug::getInstance().drawSquare(
                glm::vec2(other->x, other->y),
                glm::vec2(0.5f, 0.5f),
                glm::vec3(1.0f, 1.0f, 1.0f),
                1.05f);
            break;
          }
          else {
            tfn::Debug::getInstance().drawSquare(
                glm::vec2(other->x, other->y),
                glm::vec2(0.5f, 0.5f),
                glm::vec3(0.0f, 1.0f, 1.0f),
                1.05f);
          }
        }

        glm::ivec2 target = checkBuffer[moveTo];

        other->absPos = glm::vec2(target.x, target.y) + glm::normalize(collidedParticles[other]->velocity);
        other->lastPos = glm::vec2(other->x, other->y);
        other->lastPosI = glm::ivec2(other->x, other->y);
        other->color = glm::vec3(1.0f, 1.0f, 1.0f); // Mark as updated

        other->x = other->absPos.x;
        other->y = other->absPos.y;

        other->hasUpdated = true;       // Mark as updated
        other->velocity += p->velocity; // Acquire velocity from the moving particle
        // other->assign();
      }

      // Update particle positions in the grid
      for (tfn::Particle *other : _particlesAlongPathUpdated)
      {
        other->assign();
      }

      for (auto &p : body->particles)
      {
        p->assign();
      }
    }
  }

  void Solver::debugDraw()
  {

    for (int i = 0; i < bodyCount; i++)
    {
      SBody &b = *bodies[i];
      for (const auto &p : b.shape.points)
      {
        tfn::Debug::getInstance().drawSquare(p.pos, glm::vec2(1, 1));
      };

      for (int i = 0; i < b.shape.mesh.size(); i += 3)
      {
        int pA = b.shape.mesh[i + 0];
        int pB = b.shape.mesh[i + 1];
        int pC = b.shape.mesh[i + 2];

        glm::vec2 posA = b.shape.points[pA].pos;
        glm::vec2 posB = b.shape.points[pB].pos;
        glm::vec2 posC = b.shape.points[pC].pos;

        tfn::Debug::getInstance().drawLine(posA, posB);
        tfn::Debug::getInstance().drawLine(posC, posB);
        tfn::Debug::getInstance().drawLine(posA, posC);
      }
      /*for (const auto& c : b.constraints) {*/
      /*  if (c.type == ConstraintType::SPRING)*/
      /*    tfn::Debug::getInstance().drawLine*/
      /*      (c.constraint.spring.a->pos, c.constraint.spring.b->pos, glm::vec3(0, 1.0, 0.0));*/
      /*};*/
    };
  };
}
