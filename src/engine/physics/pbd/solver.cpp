#include "engine/physics.h"
#include "engine/engine.h"
#include "engine/debug.h"
#include "engine/math.h"
#include "tfn/particles/solid_particle.h"
#include <vector>
#include <iostream>

namespace physics {

  Solver::Solver() {

  }

  Solver::~Solver() {
    for (int i = 0; i < bodyCount; i++) {
      delete bodies[i];
    }
  }
  
  SBody* createBoxBody (glm::vec2 center, glm::vec2 size) {
    // Debug Body for now
    tfn::Square square = {
      .center = center, 
      .size = size 
    };

    const glm::vec2 tR = square.center + square.size * (float)0.5;
    const glm::vec2 tL = square.center + glm::vec2(-square.size.x/2, square.size.y/2);
    const glm::vec2 bR = square.center + glm::vec2(square.size.x/2, -square.size.y/2);
    const glm::vec2 bL = square.center - square.size * (float)0.5;

    SBody* pBody = new SBody();
    SBody& body = *pBody;

    body.shape.points.reserve(4);
    body.shape.points.push_back ({
        .pos = tR
        });

    body.shape.points.push_back ({
        .pos = tL
        });

    body.shape.points.push_back ({
        .pos = bR
        });

    body.shape.points.push_back ({
        .pos = bL
        });


    // shape
    body.shape.mesh.push_back(0);
    body.shape.mesh.push_back(1);
    body.shape.mesh.push_back(2);

    body.shape.mesh.push_back(3);
    body.shape.mesh.push_back(2);
    body.shape.mesh.push_back(1);

    // constraints
    Constraint aC = {.type = ConstraintType::SPRING };
    Constraint bC = {.type = ConstraintType::SPRING };
    Constraint cC = {.type = ConstraintType::SPRING };
    Constraint dC = {.type = ConstraintType::SPRING };
    Constraint eC = {.type = ConstraintType::SPRING };

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

  void Solver::createBodyFromPoints (std::vector<tfn::Particle*> particles) {
    glm::vec2 min;
    min.x = 99999;
    min.y = 99999;

    glm::vec2 max;
    max.x = 0;
    max.y = 0;

    for (const auto& p : particles) {
      min = glm::min(min, glm::vec2(p->x,p->y));
      max = glm::max(max, glm::vec2(p->x,p->y));
    }

    glm::vec2 center = (max+min)/(float)2;
    SBody* b = createBoxBody(center, max - min);

    // Figure out which triangles and where each particle belongs to


    for (tfn::Particle* p : particles) {
      
      glm::vec2 particlePos (p->x, p->y);
      tfn::particles::SolidParticle* solid = static_cast<tfn::particles::SolidParticle*>(p); 

      for (int i = 0; i < b->shape.mesh.size(); i+=3) {
        int pA = b->shape.mesh[i + 0];
        int pB = b->shape.mesh[i + 1];
        int pC = b->shape.mesh[i + 2];

        glm::vec2 posA = b->shape.points[pA].pos;
        glm::vec2 posB = b->shape.points[pB].pos;
        glm::vec2 posC = b->shape.points[pC].pos;

        math::BarycentricResult res = math::barycentricFromWorld(posA, posB, posC, particlePos);

        if (res.inside) {
          solid->bary = res.bary;
          solid->triIndex = i/3;
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

  void Solver::init () {

  };

  void Solver::simulate (float dt) {
    int subSteps = 5;
    float subDt = dt / (float)subSteps;

    for (int step = 0; step < subSteps; step++) {
        for (int i = 0; i < bodyCount; i++) {
            SBody& b = *bodies[i];

            // 1. Apply gravity (predict positions)
            for (auto& p : b.shape.points) {
                p.vel += glm::vec2(0, -9.81f * 10.0f) * subDt;
                p.prevPos = p.pos;                      // store for velocity update later
                p.pos += p.vel * subDt;                 // predicted position
            }

            // 2. Solve constraints (iterative position projection)
            int iterations = 5; // tweak for stiffness
            for (int iter = 0; iter < iterations; iter++) {
                for (auto& c : b.constraints) {
                    if (c.type == ConstraintType::SPRING)
                        c.constraint.spring.project();  // direct PBD projection
                }
            }

            // 3. Update velocities after projection
            for (auto& p : b.shape.points) {
                p.vel = (p.pos - p.prevPos) / subDt;
                /*p.pos = glm::clamp(p.pos, glm::vec2(-400, -400), glm::vec2(400, 400));*/

                if (p.pos.y < 0) {
                  p.pos.y = 0;
                  p.vel.y *= -0.75;
                }
            }
        }
    }

  }

  // Set particle positions and handle particle overlaps
  void Solver::propogate() {
    for (int bi = 0; bi < bodyCount; bi++) {
      SBody* body = bodies[bi];

      for (auto& p : body->particles) {
        int i = p->triIndex * 3;
        int pA = body->shape.mesh[i + 0];
        int pB = body->shape.mesh[i + 1];
        int pC = body->shape.mesh[i + 2];

        glm::vec2 posA = body->shape.points[pA].pos;
        glm::vec2 posB = body->shape.points[pB].pos;
        glm::vec2 posC = body->shape.points[pC].pos;

        glm::vec2 newPos = math::worldFromBarycentric(posA, posB, posC, p->bary);

        p->absPos.x = newPos.x;
        p->absPos.y = newPos.y;
        p->x = glm::round( newPos.x );
        p->y = glm::round( newPos.y );
        
        p->hasUpdated = true;
        p->assign();
      }

    }
  }

  void Solver::debugDraw () {

    for (int i = 0; i < bodyCount; i++) {
      SBody& b = *bodies[i];
      for (const auto& p : b.shape.points) {
        tfn::Debug::getInstance().drawSquare
          (p.pos, glm::vec2(5, 5));
      };

      for (int i = 0; i < b.shape.mesh.size(); i+=3) {
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
