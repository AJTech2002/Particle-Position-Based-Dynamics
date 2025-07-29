#include "engine/engine.h"
#include "engine/physics.h"
#include "engine/debug.h"
#include "tfn/renderer.h"
#include "tfn/grid.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

namespace game
{
  const unsigned int SCR_WIDTH = 800;
  const unsigned int SCR_HEIGHT = 800;
  const unsigned int GRID_WIDTH = 100;
  const unsigned int GRID_HEIGHT = 100;

  tfn::ComputeRenderer renderer;
  tfn::ParticleGrid particleGrid(GRID_WIDTH, GRID_HEIGHT);
  physics::Solver solver;

  void Engine::init(void)
  {
    srand(static_cast<unsigned>(time(0)));
    renderer.init(SCR_WIDTH, SCR_HEIGHT, GRID_WIDTH, GRID_HEIGHT);
    solver.init();
    tfn::Debug::getInstance().engine = this;
  }

  int brushType = 0; // 0 empty, 1 solid, 2 liquid, 3 sand
  bool pressingBrush = false;
  bool mouseDown = false;
  int brushSize = 2; // Default brush size
  int _frame = 0;
  float mx = 0.0f;
  float my = 0.0f;
  bool simulate = true;
  
  const double fixedTimestep = 1.0 / 60.0;
  double accumulatedFixedTimestep;

  std::vector<tfn::Particle*> solidParticles;

  bool step = false;

  void Engine::frame(void)
  {

    dt = (float)sapp_frame_duration();
    gameTime += dt;

    if (mouseDown)
    {
      int x = static_cast<int>(mx * GRID_WIDTH / sapp_width());
      int y = GRID_HEIGHT - static_cast<int>(my * GRID_HEIGHT / sapp_height());

      if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT)
      {

        if (brushSize > 1)
        {
          for (int i = -brushSize; i <= brushSize; ++i)
          {
            for (int j = -brushSize; j <= brushSize; ++j)
            {
              int nx = x + i;
              int ny = y + j;
              if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT)
              {
                tfn::Particle* particle = particleGrid.addParticle(nx, ny, brushType);
                if (brushType == 1) {
                  if (particle != nullptr)
                    solidParticles.push_back(particle);
                }
              }
            }
          }
        }
        else
        {
          tfn::Particle* particle = particleGrid.addParticle(x, y, brushType);
          if (brushType == 1) {
            if (particle != nullptr)
              solidParticles.push_back(particle);
          }
        }
      }
    }
    else {
      if (solidParticles.size() > 0) {
        solver.createBodyFromPoints(solidParticles);
        solidParticles.clear();
      }
    }

    accumulatedFixedTimestep += dt;

    if (step) {
      simulate = true;
      accumulatedFixedTimestep = fixedTimestep; // Force a single step
    }

    while (accumulatedFixedTimestep >= fixedTimestep) {
      if (simulate) {
          solver.simulate(dt);
        }
        
        particleGrid.update(dt, simulate);
        
        if (simulate) {
          solver.propogate(dt);
        }

      accumulatedFixedTimestep -= fixedTimestep;
    }

    if (step) {
      step = false;
      simulate = false;
    }

    renderer.update(particleGrid.display);
    solver.debugDraw();
    renderer.render();
    
    _frame++;
  }

  void Engine::cleanup(void)
  {
    particleGrid.cleanup(); // Clean up particles in the grid
    sg_shutdown();
  }

  void Engine::event(const sapp_event *event)
  {
    switch (event->type)
    {
      case SAPP_EVENTTYPE_KEY_DOWN:
        if (event->key_code == SAPP_KEYCODE_ESCAPE)
        {
          sapp_request_quit();
        }

        if (event->key_code == SAPP_KEYCODE_B && !pressingBrush)
        {
          pressingBrush = true;
          brushType = (brushType + 1) % 4;

          switch (brushType)
          {
            case 0:
              std::cout << "Brush set to EMPTY_CELL\n";
              break;
            case 1:
              std::cout << "Brush set to SOLID_CELL\n";
              break;
            case 2:
              std::cout << "Brush set to LIQUID_CELL\n";
              break;
            case 3:
              std::cout << "Brush set to SAND_CELL\n";
              break;
          }
        }

        if (event->key_code == SAPP_KEYCODE_P)
        {
          simulate = !simulate;
          if (simulate) {
            std::cout << "Simulation started\n";
          } else {
            std::cout << "Simulation paused\n";
          }
        }

        if (event->key_code == SAPP_KEYCODE_N)
        {
          if (!simulate) {
            std::cout << "Simulation step\n";
            step = true;
          } else {
            simulate = false;
            step = true;
          }
        }
        break;

      case SAPP_EVENTTYPE_KEY_UP:
        if (event->key_code == SAPP_KEYCODE_B)
        {
          pressingBrush = false;
        }
        break;

      case SAPP_EVENTTYPE_MOUSE_DOWN:
        if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT)
        {
          solidParticles.clear();
          mouseDown = true;
        }
        break;

      case SAPP_EVENTTYPE_MOUSE_UP:
        if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT)
        {
          mouseDown = false;

          // Create a RBody out of the particles
        }
        break;

      case SAPP_EVENTTYPE_MOUSE_MOVE:
          mx = event->mouse_x;
          my = event->mouse_y;

      default:
        break;
    }
  }

}
