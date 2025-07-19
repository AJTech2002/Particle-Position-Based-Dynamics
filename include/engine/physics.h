#pragma once
#ifndef PHYSICS_ENG
#define PHYSICS_ENG

#include "glm/glm.hpp"
#include <vector>

namespace physics {

  static constexpr unsigned int MAX_BODY_COUNT = 500;

  struct PointMass {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec2 force;
    float mass = 1.0;
    glm::vec2 prevPos;
  };

  struct Shape {
    std::vector<PointMass> points;

  };

  enum ConstraintType {
    SPRING = 0
  };

  struct SpringConstraint {
    PointMass* a;
    PointMass* b;
    float restDistance;
    float k = 1.0;

    void init(PointMass* _a, PointMass* _b) {
      a = _a;
      b = _b;
      if (a != nullptr && b != nullptr) {
        restDistance = glm::distance(a->pos, b->pos);
      }
    }

    void apply() {
      glm::vec2 delta = b->pos - a->pos;
      float L = glm::length(delta);

      if (L != 0) {
        glm::vec2 dir = delta / L;                   // normalize
        glm::vec2 force = -k * (L - this->restDistance) * dir;
        // apply to p1 and p2
        b->force += force;
        a->force -= force;
      }
    }

    void project() {
       glm::vec2 delta = b->pos - a->pos;
        float dist = glm::length(delta);
        if (dist == 0) return;

        glm::vec2 dir = delta / dist;

        float w1 = 1.0f / a->mass;
        float w2 = 1.0f / b->mass;
        float invMassSum = w1 + w2;
        if (invMassSum == 0) return;

        glm::vec2 correction = (dist - restDistance) / invMassSum * dir;

        // Apply only a portion based on stiffness
        correction *= k;

        a->pos += w1 / invMassSum * correction;
        b->pos -= w2 / invMassSum * correction;   }

  };

  struct Constraint {
    ConstraintType type;
    union {
     SpringConstraint spring; 
    } constraint;
  };

  struct SBody {
    Shape shape;
    std::vector<Constraint> constraints;
  };

  class Solver {
    public:
      SBody* bodies[MAX_BODY_COUNT];
      int bodyCount;

      void init ();
      void simulate (float dt);
      void debugDraw ();
  };
  
}

#endif
