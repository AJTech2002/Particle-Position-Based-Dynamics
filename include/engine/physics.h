#pragma once
#ifndef PHYSICS_ENG
#define PHYSICS_ENG

#include "glm/glm.hpp"
#include <vector>

namespace tfn {
  class Particle;
  namespace particles {
    class SolidParticle;
  }
}

namespace physics {

  static constexpr unsigned int MAX_BODY_COUNT = 500;

  struct PointMass {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec2 force;
    glm::vec2 lastForce;
    float mass = 0.0f;
    glm::vec2 prevPos;
  };

  struct Shape {
    std::vector<PointMass> points;
    std::vector<int> mesh; // This is a tri interpretation of the mesh for keeping particles in the shape
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

  struct BBox {
    glm::vec2 min;
    glm::vec2 max;
    glm::vec2 center() const {
      return (min + max) * 0.5f;
    }

    BBox() : min(glm::vec2(999999.0f)), max(glm::vec2(-999999.0f)) {}

    BBox(const glm::vec2 &min, const glm::vec2 &max) : min(min), max(max) {}

    void expand(const glm::vec2 &point) {
      min = glm::min(min, point);
      max = glm::max(max, point);
    }

    bool contains(const glm::vec2 &point) const {
      return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
    }
  };

  struct SBody {
    Shape shape;
    BBox bbox;
    std::vector<Constraint> constraints;
    std::vector<tfn::particles::SolidParticle*> particles;
  };

  class Solver {
    public:
      Solver();
      ~Solver();
    
      SBody* bodies[MAX_BODY_COUNT];
      int bodyCount;

      void init ();
      void simulate (float dt);
      void propogate (float dt);
      void debugDraw ();
      void renderOnTop();
      void createBodyFromPoints (std::vector<tfn::Particle*> particle);
  };
  
}

#endif
