#pragma once
#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "sokol/sokol_all.h"
  
  
namespace game {
  struct alignas(4) particle {
    int type;
  };

  class Engine {
    public:
      void init(void);
      void frame(void);
      void cleanup(void);
      void event (const sapp_event* event);
      void updateGrid(particle* newGrid);

      float gameTime = 0.0;
      float dt = 0.0;

      static Engine& getInstance() {
        static Engine instance;
        return instance;
      }
  };

}

#endif // ENGINE_ENGINE_H
