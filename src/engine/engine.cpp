#include "engine/engine.h"
#include "engine/physics.h"
#include "engine/debug.h"
#include "tfn/renderer.h"
#include "tfn/grid.h"
#include <cstdlib>
#include <iostream>

namespace game
{
  const unsigned int SCR_WIDTH = 800;
  const unsigned int SCR_HEIGHT = 800;
  const unsigned int GRID_WIDTH = 400;
  const unsigned int GRID_HEIGHT = 400;

  tfn::ComputeRenderer renderer;
  tfn::ParticleGrid particleGrid(GRID_WIDTH, GRID_HEIGHT);
  physics::Solver solver;

  void Engine::init(void)
  {
    renderer.init(SCR_WIDTH, SCR_HEIGHT, GRID_WIDTH, GRID_HEIGHT);
    solver.init();
  }

  int brushType = 0; // 0 empty, 1 solid, 2 liquid, 3 sand
  bool pressingBrush = false;
  bool mouseDown = false;
  int brushSize = 3; // Default brush size
  int _frame = 0;
  float mx = 0.0f;
  float my = 0.0f;
  float dt = 0.0f;

  void Engine::frame(void)
  {

    dt = (float)sapp_frame_duration();

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
                particleGrid.addParticle(nx, ny, brushType);
              }
            }
          }
        }
        else
        {
          particleGrid.addParticle(x, y, brushType);
        }
      }
    }

    solver.simulate(dt);
    solver.debugDraw();

    particleGrid.update();
    
    renderer.update(particleGrid.display);
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
          mouseDown = true;
        }
        break;

      case SAPP_EVENTTYPE_MOUSE_UP:
        if (event->mouse_button == SAPP_MOUSEBUTTON_LEFT)
        {
          mouseDown = false;
        }
        break;

      case SAPP_EVENTTYPE_MOUSE_MOVE:
        if (mouseDown)
        {
          mx = event->mouse_x;
          my = event->mouse_y;
        }

      default:
        break;
    }
  }

}
