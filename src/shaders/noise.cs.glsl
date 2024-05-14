#version 460
uniform int dim;
uniform float scale;

uniform int octaves = 10;
uniform float persistance = 0.5;
uniform float lacunarity = 2;

uniform int seed;

layout(local_size_x = 8, local_size_y = 8) in;

layout(std430, binding = 0) buffer noise_map {
    float map[];
} height_map;

layout(std430, binding = 1) buffer Height_Data {
    int min_height;
    int max_height;
} height_data;

int half_dim = dim / 2;

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : stegu
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//               https://github.com/stegu/webgl-noise
//

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x * 34.0) + 10.0) * x);
}

float noise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187, // (3.0-sqrt(3.0))/6.0
            0.366025403784439, // 0.5*(sqrt(3.0)-1.0)
            -0.577350269189626, // -1.0 + 2.0 * C.x
            0.024390243902439); // 1.0 / 41.0
    // First corner
    vec2 i = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    // Other corners
    vec2 i1;
    //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    //i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
                + i.x + vec3(0.0, i1.x, 1.0));

    vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    // Compute final noise value at P
    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

void main() {
    ivec2 noise_coord = ivec2(gl_GlobalInvocationID.xy);
    unsigned int index = noise_coord.y * dim + noise_coord.x;

    float amplitude = 1;
    float frequency = 1;
    float height = 0;

    for (int i = 0; i < octaves; i++) {
        vec2 sample_coords = vec2(float(noise_coord.x - half_dim) / scale * frequency + seed, float(noise_coord.y - half_dim) / scale * frequency + seed);
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
