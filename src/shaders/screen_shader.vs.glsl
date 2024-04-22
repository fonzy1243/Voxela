#version 460 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec4 color;

out vec4 f_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    f_color = color;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
