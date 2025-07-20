#pragma once
#ifndef MATH_PROC
#define MATH_ENG

#include <glm/glm.hpp>

using namespace glm;

namespace math {

  struct BarycentricResult {
    vec3 bary;   // (u, v, w)
    bool inside;
  };

  static BarycentricResult barycentricFromWorld (vec2& A, vec2& B, vec2& C, vec2& P) {
    vec2 v0 = B - A;
    vec2 v1 = C - A;
    vec2 v2 = P - A;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom; // W_b
    float w = (d00 * d21 - d01 * d20) / denom; // W_c
    float u = 1.0f - v - w; // Weight of P relative to A

    bool inside = (u >= 0.0f && v >= 0.0f && w >= 0.0f);

    return { vec3(u, v, w), inside };

  }

  static vec2 worldFromBarycentric (vec2& A, vec2& B, vec2& C, vec3& barycentricCoords) {
    return barycentricCoords.x * A + barycentricCoords.y * B + barycentricCoords.z * C;
  }

}

#endif
