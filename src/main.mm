#define SOKOL_IMPL
#define SOKOL_GFX_IMPL
#define SOKOL_APP_IMPL
#define SOKOL_METAL

#include <iostream>
#include <thread>

#include "engine/engine.h"

game::Engine engine;

void init_cb()    { engine.init(); }
void frame_cb()   { engine.frame(); }
void cleanup_cb() { engine.cleanup(); }
void event_cb(const sapp_event* ev) { engine.event(ev); }

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc){
        .init_cb = init_cb,
        .frame_cb = frame_cb,
        .cleanup_cb = cleanup_cb,
        .event_cb = event_cb,
        .width = 800,
        .height = 800,
        .high_dpi = true,
        .window_title = "Rendering - LearnOpenGL",
    };
}