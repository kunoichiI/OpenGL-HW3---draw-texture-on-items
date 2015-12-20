#version 410 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 tPosition;
layout(location = 2) in vec3 Pu;
layout(location = 3) in vec3 Pv;


uniform mat4 MVP;
uniform mat4 V;
uniform sampler2D bump;
out vec2 vs_tex_coord;
out vec3 normal;
out vec3 pu;
out vec3 pv;
/*out vec3 a;
out vec3 d;
out vec3 s;*/
void
main()
{
    vs_tex_coord = tPosition;
    normal = normalize(cross(Pu, Pv));
    normal = (vec4(normal, 1.0) * V).xyz;
    pu = Pu;
    pv = Pv;
    gl_Position = MVP * vPosition;
}
