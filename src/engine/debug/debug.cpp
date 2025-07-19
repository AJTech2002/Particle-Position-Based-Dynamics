#include "engine/debug/debug.h"
#include "engine/engine.h"

void tfn::Debug::drawLine (glm::vec2 from, glm::vec2 to, glm::vec3 col) {
  getInstance().submit({
      .col = col,
      .shape = {
        .line = {
          .from = from,
          .to = to
        }
      }
   });
}

inline void tfn::Debug::submit (DebugElement el) {
  if (debugBufferCount < MAX_DEBUG_BUFFER) {
    debugBuffer[debugBufferCount] = el;
    debugBufferCount += 1;
  }
}

void tfn::Debug::render() {
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
  }

  // clear memory
  debugBufferCount = 0;
}
