#pragma once
#include "engine/math.h"
#include "tfn/particle.h"
#include "engine/physics.h"
namespace tfn::particles
{
  class SolidParticle : public Particle
  {
  public:
    SolidParticle(unsigned int x, unsigned int y, int type) : Particle(x, y, type) {}

    //PHYSICS
    physics::SBody* body;
    glm::vec2 force;

    // BODY RENDERING
    int triIndex;
    glm::vec3 bary;

    void simulate() override {
      if (body == nullptr)
        Particle::simulate();
     
      /*if (body != nullptr) {*/
      /*  int i = triIndex * 3;*/
      /*  int pA = body->shape.mesh[i + 0];*/
      /*  int pB = body->shape.mesh[i + 1];*/
      /*  int pC = body->shape.mesh[i + 2];*/
      /**/
      /*  glm::vec2 posA = body->shape.points[pA].pos;*/
      /*  glm::vec2 posB = body->shape.points[pB].pos;*/
      /*  glm::vec2 posC = body->shape.points[pC].pos;*/
      /**/
      /*  glm::vec2 newPos = math::worldFromBarycentric(posA, posB, posC, bary);*/
      /**/
      /*  absPos.x = newPos.x;*/
      /*  absPos.y = newPos.y;*/
      /**/
      /*}*/
      /**/
      /*x = (int)absPos.x;*/
      /*y = (int)absPos.y;*/
    }

  };
}
