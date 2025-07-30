#pragma once
#ifndef MATH_PROC
#define MATH_ENG

#include <glm/glm.hpp>
#include <iostream>

using namespace glm;

namespace math {

  struct BarycentricResult {
    vec3 bary;   // (u, v, w)
    bool inside;
  };

  static float fastRand01() {
    return (float)rand() / RAND_MAX;
  }

  static float fastRandBetween(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
  }
  
  static int randMultiplier () {
    return rand() % 2 == 0 ? -1 : 1;
  }

  static int getLinePixels(
      glm::ivec2 a, 
      glm::ivec2 b,
      glm::ivec2* out,
      int maxSize = 1000
      ) {
    int x0 = a.x;
    int x1 = b.x;

    int y0 = a.y;
    int y1 = b.y;

    int count = 0;
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (true) {

      if (count >= maxSize) {
        std::cerr << "Warning: getLinePixels overflow, max size reached." << std::endl;
        break; // Prevent overflow
      }

      out[count++] = {x0, y0};
      if (x0 == x1 && y0 == y1) break;
      e2 = 2 * err;
      if (e2 >= dy) { err += dy; x0 += sx; }
      if (e2 <= dx) { err += dx; y0 += sy; }
    }
    return count; // size of filled array
  }

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
