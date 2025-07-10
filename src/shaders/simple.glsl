@vs vs
in vec4 position;
out vec2 vertPos;
void main() {
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    vertPos = position.xy * 0.5 + 0.5; // Normalize to [0, 1] range
}
@end

@fs fs
out vec4 FragColor;
in vec2 vertPos;
layout(binding = 0) uniform texture2D tex;
layout(binding = 0) uniform sampler sampler_tex;

void main() {
    vec4 color = texture(sampler2D(tex, sampler_tex), vertPos.xy);
    FragColor = color;
}
@end

@program simple vs fs