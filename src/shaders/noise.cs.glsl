#version 460
uniform int dim;
uniform float scale;

uniform int octaves = 4;
uniform float persistance = 0.5;
uniform float lacunarity = 2;

uniform int seed;

layout(local_size_x = 4, local_size_y = 4) in;

layout(std430, binding = 0) buffer noise_map {
    float map[];
} height_map;

layout(std430, binding = 1) buffer Height_Data {
    int min_height;
    int max_height;
} height_data;

float mod289(float x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
vec4 permute(vec4 x) {
    return mod289(((x * 34.0) + 1.0) * x);
}
//	Classic Perlin 2D Noise
//	by Stefan Gustavson
//
vec2 fade(vec2 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float noise(vec2 P) {
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;
    vec4 i = permute(permute(ix) + iy);
    vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;
    vec2 g00 = vec2(gx.x, gy.x);
    vec2 g10 = vec2(gx.y, gy.y);
    vec2 g01 = vec2(gx.z, gy.z);
    vec2 g11 = vec2(gx.w, gy.w);
    vec4 norm = 1.79284291400159 - 0.85373472095314 *
                vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;
    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}

void main() {
    ivec2 noise_coord = ivec2(gl_GlobalInvocationID.xy);
    unsigned int index = noise_coord.y * dim + noise_coord.x;

    float amplitude = 1;
    float frequency = 1;
    float height = 0;

    for (int i = 0; i < octaves; i++) {
        vec2 sample_coords = vec2(float(noise_coord.x) / scale * frequency + seed, float(noise_coord.y) / scale * frequency + seed);
        float noise_value = noise(sample_coords);
        height += noise_value * amplitude;

        amplitude *= persistance;
        frequency *= lacunarity;
    }

    height_map.map[index] = height;

    atomicMin(height_data.min_height, int(height * 100000));
    atomicMax(height_data.max_height, int(height * 100000));

    barrier();

    height_map.map[index] = smoothstep(float(height_data.min_height) / 100000, float(height_data.max_height) / 100000, height);
}
