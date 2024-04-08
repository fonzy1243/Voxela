#version 460 core
out vec4 ScreenColor;

uniform int screen_width;
uniform int screen_height;

float sphere_sdf(vec3 p, float r) {
    return length(p) - r;
}

float box_sdf(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float scene_sdf(vec3 p) {
    return box_sdf(p, vec3(0.5, 0.5, 0.5));
}

vec3 get_normals(vec3 p) {
    float EPSILON = 0.001;
    return normalize(vec3(
            scene_sdf(vec3(p.x + EPSILON, p.yz)) - scene_sdf(vec3(p.x - EPSILON, p.yz)),
            scene_sdf(vec3(p.x, p.y + EPSILON, p.z)) - scene_sdf(vec3(p.x, p.y - EPSILON, p.z)),
            scene_sdf(vec3(p.xy, p.z + EPSILON)) - scene_sdf(vec3(p.xy, p.z - EPSILON))
        ));
}

float soft_shadow(vec3 p, vec3 rd, float mint, float maxt, float w) {
    float res = 1.0;
    float t = mint;
    for (int i = 0; i < 256 && t < maxt; i++) {
        float h = scene_sdf(p + t * rd);
        res = min(res, h / (w * t));
        if (res < -1.0 || t > maxt) break;
    }

    res = max(res, -1.0);
    return 0.25 * (1.0 + res) * (1.0 + res) * (2.0 - res);
}

vec3 ray_march(vec3 pos, vec3 ray_dir) {
    const int MAX_STEP = 64;

    vec3 light_source = vec3(-1.0, 5.0, 2.0);

    int step = 0;
    float d = scene_sdf(pos);

    while (abs(d) > 0.001 && step < MAX_STEP) {
        pos = pos + ray_dir * d;
        d = scene_sdf(pos);
        step++;
    }

    if (step < MAX_STEP) {
        vec3 normal = get_normals(pos);
        vec3 light_dir = normalize(light_source - pos);
        float diff = max(dot(normal, light_dir), 0.0);
        float shadow = soft_shadow(pos, light_dir, 0.1, 3.0, 16.0);
        return vec3(0.65, 0.1, 0.2) * diff * shadow;
    } else {
        return vec3(0, 0, 0);
    }
}

vec3 ray_dir(float fov, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(radians(fov) / 2.0);
    return normalize(vec3(xy, -z));
}

void main() {
    vec2 screen = vec2(screen_width, screen_height);

    float gamma = 2.2;

    vec3 pos = vec3(-1.0, 0.0, 5.0);
    vec3 r_dir = ray_dir(45.0, screen, gl_FragCoord.xy);
    vec3 rgb = ray_march(pos, r_dir);
    rgb = pow(rgb, vec3(1.0 / gamma));
    ScreenColor = vec4(rgb, 1.0f);
}
