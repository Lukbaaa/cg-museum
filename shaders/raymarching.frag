#version 330 core

uniform float angle;
uniform float time;
out vec4 FragColor;

mat4 rotatex(float a) {
    a = radians(a);
    float c = cos(a);
    float s = sin(a);

    return mat4(
        vec4(1, 0, 0, 0),
        vec4(0, c, -s, 0),
        vec4(0, s, c, 0),
        vec4(0, 0, 0, 1)
    );
}

mat4 rotatey(float a) {
    a = radians(a);
    float c = cos(a);
    float s = sin(a);

    return mat4(
        vec4(c, 0, s, 0),
        vec4(0, 1, 0, 0),
        vec4(-s, 0, c, 0),
        vec4(0, 0, 0, 1)
    );
}

vec4 blend(vec4 a, vec4 b, float k )
{
    float h = clamp(0.5+0.5*(b.w-a.w)/k, 0.0, 1.0);
    float blendDst = mix(b.w, a.w, h) - k*h*(1.0-h);
    vec3 blendCol = mix(b.xyz, a.xyz, h);
    return vec4(blendCol, blendDst);
}

vec4 sdfSphere(vec3 p, vec3 c, float r)
{
    return vec4(1,0,0,length(p - c) - r);
}

vec4 sdfCube(vec3 p, vec3 c, vec3 size) {
    vec3 o = abs(p-c) -size;
    float ud = length(max(o,0));
    float n = max(max(min(o.x,0),min(o.y,0)), min(o.z,0));
    return vec4(0,0,1,ud+n);
}

vec4 scene(vec3 p)
{
    vec4 sphere = sdfSphere(p, vec3(sin(time)*1.5,0,0), 0.5);
    p = (inverse(rotatex(45)) * inverse(rotatey(45)) * vec4(p, 1.0)).xyz; 
    vec4 cube = sdfCube(p, vec3(0,0,0), vec3(0.25));

    return blend(sphere, cube, 0.5);
}

vec3 calculate_normal(vec3 p)
{
    const vec3 step = vec3(0.001, 0.0, 0.0);

    float x = scene(p + step.xyy).w - scene(p - step.xyy).w;
    float y = scene(p + step.yxy).w - scene(p - step.yxy).w;
    float z = scene(p + step.yyx).w - scene(p - step.yyx).w;

    vec3 normal = vec3(x, y, z);

    return normalize(normal);
}

vec4 ray_march(vec3 ro, vec3 rd)
{
    float total_distance_traveled = 0.0;
    const int NUMBER_OF_STEPS = 32;
    const float MINIMUM_HIT_DISTANCE = 0.0001;
    const float MAXIMUM_TRACE_DISTANCE = 1000.0;

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        vec3 current_position = ro + total_distance_traveled * rd;
        vec4 color_with_dist = scene(current_position);
        float distance_to_closest = color_with_dist.w;

        if (distance_to_closest < MINIMUM_HIT_DISTANCE) 
        {
            vec3 normal = calculate_normal(current_position);
            vec3 light_position = vec3(.0, 2.0, .0);
            vec3 direction_to_light = normalize(current_position - light_position);

            float diffuse_intensity = max(0.0, dot(normal, direction_to_light));

            return vec4(color_with_dist.xyz * (1-diffuse_intensity), 1);
        }

        if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE)
        {
            return vec4(0);
        }
        total_distance_traveled += distance_to_closest;
    }
    return vec4(0);
}

void main()
{
    vec2 uv = (gl_FragCoord.xy/800) * 2.0 - 1.0;

    vec3 ro = vec3(0,0,-3);
    vec3 rd = vec3(uv, 1.0);

    FragColor = ray_march(ro, rd);;
}