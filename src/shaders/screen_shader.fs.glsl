#version 460 core
out vec4 ScreenColor;

uniform int screen_width;
uniform int screen_height;

void main() {
    float r = gl_FragCoord.x / (screen_width - 1);
    float g = gl_FragCoord.y / (screen_height - 1);
    float b = 0;
    vec2 rg = vec2(r, g);
    ScreenColor = vec4(rg, b, 1.0f);
}
