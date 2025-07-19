#ifndef ENGINE_DEBUG_DEBUG_H
#define ENGINE_DEBUG_DEBUG_H
#include "engine/engine.h"
#include "glm/glm.hpp"

namespace tfn
{
  // Amount of debug elements that can be added per frame
  static constexpr int MAX_DEBUG_BUFFER = 200;

  enum DebugShapeType {
    LINE = 0,
    CIRCLE = 1,
    SQUARE = 2
  };

  struct Line {
    glm::vec2 from;
    glm::vec2 to;
  };

  struct Square {
    glm::vec2 center;
    glm::vec2 size;
  };

  struct Circle {
    glm::vec2 center;
    float radius;
  };

  struct DebugElement {
    DebugShapeType type; 
    glm::vec3 col;

    union {
      Line line;
      Circle circle;
      Square square;
    } shape;
  };

  class Debug {
    public:
      static Debug& getInstance() {
        static Debug instance;
        return instance;
      }
     
      static void drawLine(glm::vec2 from, glm::vec2 to, glm::vec3 col = glm::vec3(1,0,0));
      static void drawSquare(glm::vec2 center, glm::vec2 extent, glm::vec3 col = glm::vec3(1,0,0));

      void render();
    private:
      int debugBufferCount;
      DebugElement debugBuffer[MAX_DEBUG_BUFFER];
      inline void submit (DebugElement el);
  };
}

#endif

