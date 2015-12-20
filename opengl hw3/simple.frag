#version 410 core

struct Light{
    vec3 position;
    vec3 diffuse;
};
uniform sampler2D tex;
uniform sampler2D bump;
uniform Light light;
uniform mat4 V;
uniform float u;
uniform int t;
uniform int l;
uniform int b;
in vec2 vs_tex_coord;
in vec3 normal;
in vec3 pu;
in vec3 pv;
out vec4 color;

void
main()
{
    vec2 o1 =  vs_tex_coord + vec2(u, 0);
    vec2 o2 =  vs_tex_coord + vec2(0, u);
    float du = (texture(bump, o1).r - texture(bump, vs_tex_coord).r);
    du = du / u;
    float dv = (texture(bump, o2).r - texture(bump, vs_tex_coord).r) ;
    dv = dv / u;
    vec3 n = normal + du * cross(normal, pv) + dv * cross(pu, normal);
    n = normalize(n);
 
 //   vec3 n = normal;
    vec3 fcolor = vec3(0, 0, 0);
    vec3 p = normalize(light.position);
    float diffuse = max(dot(p, n), 0.0);
    vec3 d = vec3(0.5, 0.5, 0.5);
    if (l == 1) {
        fcolor += d * light.diffuse;
    }
   
    if (t == 1) {
        fcolor = fcolor * texture(tex, vs_tex_coord).rgb;
    }
    if (b == 1) {
        fcolor = fcolor * diffuse;
    }
    
//        fcolor = d * diffuse * light.diffuse * texture(bump, vs_tex_coord).rgb;
//
//    fcolor = d * light.diffuse * texture(tex, vs_tex_coord).rgb;
//    vec3 fcolor = d * diffuse * light.diffuse ;

//    vec3 fcolor = d * diffuse * light.diffuse;

//    color = texture(bump, vs_tex_coord);
//    vec3 fcolor =texture(tex, vs_tex_coord).rgb;
    color = vec4(fcolor, 1.0);
    
    
}
