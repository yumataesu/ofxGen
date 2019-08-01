#version 410

struct Light {
    vec3 position;
    vec4 diffuse;
    //int isDirectinal;
};
const int LightNUM = 12;

uniform sampler2D color_tex;
uniform sampler2D position_tex;
uniform sampler2D ssao;
uniform mat4 view;
uniform int light_num;

uniform Light lights[LightNUM];
in vec2 v_texcoord;
in vec3 v_light_pos;

out vec4 fragColor;

vec3 calcFlatNormal(vec3 world){
    vec3 dx = dFdx(world.xyz);
    vec3 dy = dFdy(world.xyz);
    vec3 n = normalize(cross(normalize(dx), normalize(dy)));
    
    return n;
}

void main() {
    vec3 world = texture(position_tex, v_texcoord).rgb;
    vec3 color = texture(color_tex, v_texcoord).xyz;
    vec3 normal = calcFlatNormal(world.xyz).xyz;

    vec3 v_view_dir = -world;
    vec3 N = normalize(normal);

    vec4 result = vec4(0);

    for(int i = 0; i < light_num; ++i) {
        vec3 v_light_pos = (view * vec4(lights[i].position, 1.0)).xyz;
        vec3 L = normalize(v_light_pos);
        vec3 V = normalize(v_view_dir);
        vec3 H = normalize(L + V);
    
        vec3 diffuse = vec3(max(dot(N, L), 0.0)) * lights[i].diffuse.rgb; // Replace the R.V calculation (as in Phong) with N.H
        vec3 specular = vec3(pow(max(dot(N, H), 0.0), 64));

        result += vec4(diffuse + specular, 1.0);
    }

    result.rgb *= vec3(2.5);
    vec3 ao = texture(ssao, v_texcoord).rgb;
    vec3 c = pow(color.rgb, vec3(2.2)) * vec3(1.3);
    //vec3 normal = calcFlatNormal(world.xyz).xyz;
    fragColor = vec4(c.rgb * vec3(ao.r), 1.0);
    fragColor = vec4(vec3(ao.r, ao.r, ao.r), 1.0);
    fragColor = vec4(vec3(c), 1.0);

}
