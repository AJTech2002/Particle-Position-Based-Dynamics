#include "engine/debug.h"
#include "vector"

void tfn::Debug::drawLine (glm::vec2 from, glm::vec2 to, glm::vec3 col, float duration) {
  getInstance().submit({
      .col = col,
      .type = tfn::DebugShapeType::LINE,
      .duration = duration,
      .startTime = getInstance().engine->gameTime,
      .shape = {
        .line = {
          .from = from,
          .to = to
        }
      }
   });
}

void tfn::Debug::drawSquare (glm::vec2 center, glm::vec2 extent, glm::vec3 col, float duration) {
  getInstance().submit({
      .col = col,
      .type = tfn::DebugShapeType::SQUARE,
      .duration = duration,
      .startTime = getInstance().engine->gameTime,
      .shape = {
        .square = {
          .center = center,
          .size = extent
        }
      }
  });
};

void tfn::Debug::submit (DebugElement el) {
  if (debugBufferCount < MAX_DEBUG_BUFFER) {
    debugBuffer[debugBufferCount] = el;
    debugBufferCount += 1;
  }
}

void tfn::Debug::render() {

  std::vector<int> preservedElements;

  for (int i = 0; i < debugBufferCount; i++) {
    DebugElement element = this->debugBuffer[i];
    if (element.type == tfn::DebugShapeType::LINE) {
      Line line = element.shape.line;
      sgl_begin_lines();
      sgl_c3f(element.col.x, element.col.y, element.col.z);
      sgl_v3f(line.from.x, line.from.y, 0);
      sgl_v3f(line.to.x, line.to.y, 0);
      sgl_end();
    }

    else if (element.type == tfn::DebugShapeType::SQUARE) {

      Square square = element.shape.square;
      sgl_begin_line_strip();
      const glm::vec2 tR = square.center + square.size * (float)0.5;
      const glm::vec2 tL = square.center + glm::vec2(-square.size.x/2, square.size.y/2);
      const glm::vec2 bR = square.center + glm::vec2(square.size.x/2, -square.size.y/2);
      const glm::vec2 bL = square.center - square.size * (float)0.5;

      sgl_c3f(element.col.x, element.col.y, element.col.z);
      sgl_v3f (tR.x, tR.y, 0);
      sgl_v3f (tL.x, tL.y, 0);
      sgl_v3f (bL.x, bL.y, 0);
      sgl_v3f (bR.x, bR.y, 0);
      sgl_v3f (tR.x, tR.y, 0);
      sgl_end();

    }

    if (element.duration >= 0.01 && this->engine->gameTime - element.startTime < element.duration) {
      preservedElements.push_back(i);
    }

  }

  // clear memory
  debugBufferCount = 0;

  for (auto& preserved : preservedElements) {
    this->submit(debugBuffer[preserved]);
  }
}
