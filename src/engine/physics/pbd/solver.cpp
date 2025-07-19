#include "engine/physics.h"
#include "engine/engine.h"
#include "engine/debug.h"
#include <vector>
#include <iostream>

namespace physics {

  void Solver::init () {

    // Debug Body for now
    tfn::Square square = {
      .center = glm::vec2(100, 100),
      .size = glm::vec2(100, 100)
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

    bodies[0] = &body;
    bodyCount++;
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

                if (p.pos.y < -400) {
                  p.pos.y = -400;
                  p.vel.y *= -0.75;
                }
            }
        }
    }
  }

  void Solver::debugDraw () {

    for (int i = 0; i < bodyCount; i++) {
      SBody& b = *bodies[i];
      for (const auto& p : b.shape.points) {

        tfn::Debug::getInstance().drawSquare
          (p.pos, glm::vec2(10, 10));

      };

      for (const auto& c : b.constraints) {
        if (c.type == ConstraintType::SPRING)
          tfn::Debug::getInstance().drawLine
            (c.constraint.spring.a->pos, c.constraint.spring.b->pos, glm::vec3(0, 1.0, 0.0));
      };
    };

  };

}
