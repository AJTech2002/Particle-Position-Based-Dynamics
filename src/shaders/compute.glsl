#version 430

@block common
struct particle {
    int type;
};
@end

@cs cs
@include_block common

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba8, binding = 0) uniform writeonly image2D destImage;
// layout(std430, binding = 1) buffer ParticleGrid {
//     int particles[]; // Assuming particles are stored as vec4
// };

layout(std430, binding=1) readonly buffer cs_ssbo_in { particle particles[]; };


int width = 400; // Width of the particle grid, assume square

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    int particle = particles[pos.y * width + pos.x].type; // Access the particle data
    
    // TODO: Remove branching and use lookups to optimize
    if (particle == 0) {
        // If no particle, set the pixel to black

        imageStore(destImage, pos, vec4(0.0, 0.0, 0.0, 1.0));
        return;
    }
    else if (particle == 1) {
        // If particle is type 1, set the pixel to blue
        imageStore(destImage, pos, vec4(0.0, 0.0, 1.0, 1.0)); // blue
        return;
    }
    else if (particle == 2) {
        // If particle is type 2, set the pixel to green
        imageStore(destImage, pos, vec4(0.0, 1.0, 0.0, 1.0)); // green
        return;
    }


    imageStore(destImage, pos, vec4(1.0, 1.0, 0.0, 0.0)); // red
}
@end

@program compute cs