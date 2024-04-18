#version 460 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 offset;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(pos + offset, 1.0);
}
