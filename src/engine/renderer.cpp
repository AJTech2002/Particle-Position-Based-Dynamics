#include "engine/engine.h"
#include "tfn/renderer.h"
#include "tfn/grid.h"
#include "shaders/simple.glsl.h"
#include "shaders/compute.glsl.h"
#include "engine/debug.h"

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

int W = 400;
int H = 400; 
int _SCR_WIDTH = 0;
int _SCR_HEIGHT = 0;

struct alignas(16) UniformParams
{
  int width;
};

UniformParams params = {
  .width = 0
};

void tfn::ComputeRenderer::init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, unsigned int GRID_WIDTH, unsigned int GRID_HEIGHT)
{
  W = GRID_WIDTH;
  H = GRID_HEIGHT;
  _SCR_WIDTH = SCR_WIDTH;
  _SCR_HEIGHT = SCR_HEIGHT;
  
  params.width = W;

  sg_desc desc = {
    .environment = sglue_environment(),
    .logger = {.func = slog_func}};
  sg_setup(&desc);

  sgl_desc_t sglDesc = {
    .logger = {.func = slog_func},

  };

  sgl_setup(&sglDesc);

  sg_image_desc img_desc = {
    // .render_target = true, // allows imageStore/imageLoad
    .usage = {
      // .stream_update = true,
      .storage_attachment = true,
    },

    .width = W,
    .height = W,
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
      .index_buffer = true
    }  
  };

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
    .size = sizeof(DisplayCell) * W * H,
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
}

void tfn::ComputeRenderer::update (tfn::DisplayCell* cells) {
  // Update storage buffer
  sg_range data = {
    .ptr = cells,                   // pointer to CPU data
    .size = sizeof(DisplayCell) * (W * W) // size of the data
  };
  sg_update_buffer(storageBuffer, &data);
}

void tfn::ComputeRenderer::render()
{
  sg_pass pass = {
    .action = state.passAction,
    .swapchain = sglue_swapchain(),
  };

  sg_pass computePass = {.compute = true, .label = "compute-pass", .attachments = atts};

  sgl_defaults();
  sgl_matrix_mode_projection();
  sgl_load_identity();   // resets to pure NDC mapping
  sgl_matrix_mode_modelview();
  sgl_load_identity();

  // Handle Debugger

  sgl_push_pipeline();
  sgl_ortho(-(float)_SCR_WIDTH/2, (float)_SCR_WIDTH/2, -(float)_SCR_HEIGHT/2, (float)_SCR_HEIGHT/2, -1.0, 1.0);
  sgl_viewport(0, 0, _SCR_WIDTH, _SCR_HEIGHT, true);
  tfn::Debug::getInstance().render();
  sgl_pop_pipeline();

  sg_begin_pass(&computePass);

  sg_apply_pipeline(computePipeline);
  sg_apply_bindings(&computeBindings);

  // Dispatch compute shader
  int width = W;
  int height = W;
  int numGroupsX = (width + 15) / 16;  // Assuming local_size_x = 16
  int numGroupsY = (height + 15) / 16; // Assuming local_size_y = 16

  sg_apply_uniforms(2, { .ptr = &params, .size = sizeof(UniformParams) });

  sg_dispatch(numGroupsX, numGroupsY, 1);

  sg_end_pass();

  sg_begin_pass(&pass);

  sg_apply_pipeline(state.pip);
  sg_apply_bindings(&state.bindings);

  sg_draw(0, 6, 1);
  sgl_draw();

  sg_end_pass();
  sg_commit();
}
