# for each file in src/shaders
for shader in src/shaders/*; do
  # compile the shader
  ./sokol-shdc --input "$shader" --output "./include/shaders/$(basename "$shader" .glsl).glsl.h" --format sokol --slang glsl430:hlsl4:metal_macos
done