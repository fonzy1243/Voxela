#version 460

layout(location = 0) in vec2 aPos;

out vec2 TexCoords;

void main() {
    float scale = 1;
    vec2 scaled_pos = aPos * scale;
    gl_Position = vec4(aPos, 0.0, 1.0);

    TexCoords = (scaled_pos + 1.0) * 0.5;
}
