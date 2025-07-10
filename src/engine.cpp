#include "engine/engine.h"
#include "shaders/simple.glsl.h"
#include "shaders/compute.glsl.h"
#include <cstdlib>
#include <iostream>

namespace game
{

  static struct
  {
    sg_pass_action passAction;
    sg_pipeline pip;
    sg_bindings bindings;
  } state;


  sg_buffer storageBuffer;
  sg_bindings computeBindings;
  sg_pipeline computePipeline;
  sg_attachments atts;

  static constexpr int WIDTH = 400;

  particle grid[WIDTH * WIDTH];

  void Engine::init(void)
  {
    sg_desc desc = {
        .environment = sglue_environment(),
        .logger = {.func = slog_func}};
    sg_setup(&desc);

    sg_image_desc img_desc = {
        // .render_target = true, // allows imageStore/imageLoad
        .usage = {
            // .stream_update = true,
            .storage_attachment = true,
        },

        .width = WIDTH,
        .height = WIDTH,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
    };

    sg_sampler_desc samp_desc = {
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
    };

    sg_sampler sampler = sg_make_sampler(&samp_desc);
    sg_image dest_image = sg_make_image(&img_desc);


    state.passAction = (sg_pass_action){
        .colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.2f, 0.3f, 0.3f, 1.0f}}};

    sg_shader shd = sg_make_shader(simple_shader_desc(sg_query_backend()));

    float vertices[] = {
        // positions
        1.0f, 1.0f, 0.0f,   // top right
        1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f   // top left
    };

    sg_buffer_desc vertexBufferDesc = {
        .size = sizeof(vertices),
        .data = SG_RANGE(vertices),
        .label = "quad-vertices"};

    state.bindings.vertex_buffers[0] = sg_make_buffer(&vertexBufferDesc);

    uint16_t indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    sg_buffer_desc indexBufferDesc = {
        .size = sizeof(indices),
        .data = SG_RANGE(indices),
        .label = "quad-indices",
        .usage = {
            .index_buffer = true}};

    state.bindings.index_buffer = sg_make_buffer(&indexBufferDesc);
    state.bindings.images[0] = dest_image;
    state.bindings.samplers[0] = sampler;

    sg_pipeline_desc pipelineDesc = {
        .shader = shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .layout = {
            .attrs = {
                [ATTR_simple_position].format = SG_VERTEXFORMAT_FLOAT3}},
        .label = "quad-pipeline"};

    state.pip = sg_make_pipeline(&pipelineDesc);

    state.passAction = (sg_pass_action){
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR, .clear_value = {0.2f, 0.3f, 0.3f, 1.0f}}};

    sg_buffer_desc buf_desc = {
        .size = sizeof(grid),
        .usage = {
            .storage_buffer = true,
            .dynamic_update = true,
        },
    };

    storageBuffer = sg_make_buffer(&buf_desc);

    computeBindings.storage_buffers[1] = storageBuffer;

    sg_attachments_desc atts_desc = {
      .storages[0] = {
        .image = dest_image,
      }
    };

    atts = sg_make_attachments(&atts_desc);

    sg_pipeline_desc computePipelineDesc = {
        .compute = true,
        .shader = sg_make_shader(compute_shader_desc(sg_query_backend())),
        .label = "compute-pipeline"};

    computePipeline = sg_make_pipeline(computePipelineDesc);


    particle randomGrid[WIDTH * WIDTH];
    for (int x = 0; x < WIDTH; x++)
    {
      for (int y = 0; y < WIDTH; y++)
      {
        int index = x + (y * WIDTH);
        if (x == 100 || y == 20) {
          randomGrid[index].type = 1; // Blue particle
          
        }
        else {
          randomGrid[index].type = 0;
        }
      }
    }

    this->updateGrid(randomGrid);

  }

  void Engine::updateGrid(particle* newGrid)
  {
    // Update storage buffer
    sg_range data = {
        .ptr = newGrid,                   // pointer to CPU data
        .size = sizeof(particle) * (WIDTH * WIDTH) // size of the data
    };
    sg_update_buffer(storageBuffer, &data);
  }

  struct alignas(16) UniformParams
  {
    int width;
  };

  UniformParams params = {
      .width = WIDTH
  };

  void Engine::frame(void)
  {
    sg_pass pass = {
        .action = state.passAction,
        .swapchain = sglue_swapchain(),
    };

    sg_pass computePass = {.compute = true, .label = "compute-pass", .attachments = atts};

    sg_begin_pass(&computePass);

    sg_apply_pipeline(computePipeline);
    sg_apply_bindings(&computeBindings);

    // Dispatch compute shader
    int width = WIDTH;
    int height = WIDTH;
    int numGroupsX = (width + 15) / 16;  // Assuming local_size_x = 16
    int numGroupsY = (height + 15) / 16; // Assuming local_size_y = 16

    sg_apply_uniforms(2, { .ptr = &params, .size = sizeof(UniformParams) });

    sg_dispatch(numGroupsX, numGroupsY, 1);

    sg_end_pass();

    sg_begin_pass(&pass);

    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bindings);

    sg_draw(0, 6, 1);

    sg_end_pass();
    sg_commit();
  }

  void Engine::cleanup(void)
  {
    sg_shutdown();
  }

  void Engine::event(const sapp_event *event)
  {
    if (event->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
      if (event->key_code == SAPP_KEYCODE_ESCAPE)
      {
        sapp_request_quit();
      }
    }
  }

}
