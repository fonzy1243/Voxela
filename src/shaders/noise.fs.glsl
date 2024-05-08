#version 460

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D noise_texture;

void main() {
    float noise_value = texture(noise_texture, TexCoords).r;
    FragColor = vec4(vec3(noise_value), 1.0);
}
