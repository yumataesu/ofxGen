#version 410

uniform int u_layer_num;

struct Layer {
    sampler2D depth;
    sampler2D color;
    sampler2D position;
    float alpha;
};

uniform Layer layer[6];

in vec2 v_texcoord;

layout(location = 0) out vec4 fragColor_color;
layout(location = 1) out vec4 fragColor_position;
layout(location = 2) out vec4 fragColor_normal;

vec3 calcFlatNormal(vec3 world){
    vec3 dx = dFdx(world.xyz);
    vec3 dy = dFdy(world.xyz);
    vec3 n = normalize(cross(normalize(dx), normalize(dy)));
    
    return n;
}

void main() {

    if(u_layer_num == 0) {
        fragColor_color = vec4(0, 0, 0, 1);
        fragColor_position = vec4(0);
    }

    if(u_layer_num == 1) {
        float tex1_depth = texture(layer[0].depth, v_texcoord).r;
        vec4 tex1_color = texture(layer[0].color, v_texcoord);
        vec4 tex1_world = texture(layer[0].position, v_texcoord);

        vec4 world = vec4(0.0);

        gl_FragDepth = tex1_depth;
        fragColor_color = vec4(tex1_color.rgb * layer[0].alpha, layer[0].alpha);
        world = vec4(tex1_world.xyz, 1.0);
        fragColor_position = world;
        fragColor_normal = vec4(calcFlatNormal(vec3(world.x, world.y, world.z)), 1.0);

    }
    else if(u_layer_num == 2) {
        float d0 = texture(layer[0].depth, v_texcoord).r;
        float d1 = texture(layer[1].depth, v_texcoord).r;
        vec4 c0  = texture(layer[0].color, v_texcoord);
        vec4 c1  = texture(layer[1].color, v_texcoord);
        vec4 p0  = texture(layer[0].position, v_texcoord);
        vec4 p1  = texture(layer[1].position, v_texcoord);

        vec4 world = vec4(0.0);

        fragColor_color = vec4(mix(c1.rgb * layer[1].alpha, c0.rgb * layer[0].alpha, step(vec3(d0), vec3(d1))), 1.0);
        world = vec4(mix(p1.rgb, p0.rgb, step(vec3(d0), vec3(d1))), 1.0);
        fragColor_position = world;
        fragColor_normal = vec4(calcFlatNormal(vec3(world.x, world.y, world.z)), 1.0);
        gl_FragDepth = min(d0, d1);

    }
    else if(u_layer_num == 3) {
        float d0 = texture(layer[0].depth, v_texcoord).r;
        float d1 = texture(layer[1].depth, v_texcoord).r;
        float d2 = texture(layer[2].depth, v_texcoord).r;
        
        vec4 c0  = texture(layer[0].color, v_texcoord);
        vec4 c1  = texture(layer[1].color, v_texcoord);
        vec4 c2  = texture(layer[2].color, v_texcoord);

        vec4 p0  = texture(layer[0].position, v_texcoord);
        vec4 p1  = texture(layer[1].position, v_texcoord);
        vec4 p2  = texture(layer[2].position, v_texcoord);

        vec4 world = vec4(0.0);

        vec4 col_result = vec4(mix(c1.rgb * layer[1].alpha, c0.rgb * layer[0].alpha, step(vec3(d0), vec3(d1))), 1.0);
        vec4 position_result = vec4(mix(p1.rgb, p0.rgb, step(vec3(d0), vec3(d1))), 1.0);
        float depth_result = min(d0, d1);

        fragColor_color = vec4(mix(col_result.rgb, c2.rgb * layer[2].alpha, step(vec3(d2), vec3(depth_result))), 1.0);
        world = vec4(mix(position_result.rgb, p2.rgb, step(vec3(d2), vec3(depth_result))), 1.0);
        fragColor_position = world;
        fragColor_normal = vec4(calcFlatNormal(vec3(world.x, world.y, world.z)), 1.0);
        gl_FragDepth = min(depth_result, d2);
    }
    else if(u_layer_num == 4) {
        float d0 = texture(layer[0].depth, v_texcoord).r;
        float d1 = texture(layer[1].depth, v_texcoord).r;
        float d2 = texture(layer[2].depth, v_texcoord).r;
        float d3 = texture(layer[3].depth, v_texcoord).r;
        
        vec4 c0  = texture(layer[0].color, v_texcoord);
        vec4 c1  = texture(layer[1].color, v_texcoord);
        vec4 c2  = texture(layer[2].color, v_texcoord);
        vec4 c3  = texture(layer[3].color, v_texcoord);

        vec4 p0  = texture(layer[0].position, v_texcoord);
        vec4 p1  = texture(layer[1].position, v_texcoord);
        vec4 p2  = texture(layer[2].position, v_texcoord);
        vec4 p3  = texture(layer[3].position, v_texcoord);

        vec4 world = vec4(0.0);

        vec4 col_result = vec4(mix(c1.rgb * layer[1].alpha, c0.rgb * layer[0].alpha, step(vec3(d0), vec3(d1))), 1.0);
        vec4 position_result = vec4(mix(p1.rgb, p0.rgb, step(vec3(d0), vec3(d1))), 1.0);
        float depth_result = min(d0, d1);

        vec4 col_result2 = vec4(mix(col_result.rgb, c2.rgb * layer[2].alpha, step(vec3(d2), vec3(depth_result))), 1.0);
        vec4 position_result2 = vec4(mix(position_result.rgb, p2.rgb, step(vec3(d2), vec3(depth_result))), 1.0);
        float depth_result2 = min(d2, depth_result);

        fragColor_color = vec4(mix(col_result2.rgb, c3.rgb * layer[3].alpha, step(vec3(d3), vec3(depth_result2))), 1.0);
        world = vec4(mix(position_result2.rgb, p3.rgb, step(vec3(d3), vec3(depth_result2))), 1.0);
        fragColor_position = world;
        fragColor_normal = vec4(calcFlatNormal(vec3(world.x, world.y, world.z)), 1.0);
        gl_FragDepth = min(depth_result2, d3);
    }
    else if(u_layer_num == 5) {
        float d0 = texture(layer[0].depth, v_texcoord).r;
        float d1 = texture(layer[1].depth, v_texcoord).r;
        float d2 = texture(layer[2].depth, v_texcoord).r;
        float d3 = texture(layer[3].depth, v_texcoord).r;
        float d4 = texture(layer[4].depth, v_texcoord).r;
        
        vec4 c0  = texture(layer[0].color, v_texcoord);
        vec4 c1  = texture(layer[1].color, v_texcoord);
        vec4 c2  = texture(layer[2].color, v_texcoord);
        vec4 c3  = texture(layer[3].color, v_texcoord);
        vec4 c4  = texture(layer[4].color, v_texcoord);

        vec4 p0  = texture(layer[0].position, v_texcoord);
        vec4 p1  = texture(layer[1].position, v_texcoord);
        vec4 p2  = texture(layer[2].position, v_texcoord);
        vec4 p3  = texture(layer[3].position, v_texcoord);
        vec4 p4  = texture(layer[4].position, v_texcoord);

        vec4 world = vec4(0.0);

        vec4 col_result = vec4(mix(c1.rgb * layer[1].alpha, c0.rgb * layer[0].alpha, step(vec3(d0), vec3(d1))), 1.0);
        vec4 position_result = vec4(mix(p1.rgb, p0.rgb, step(vec3(d0), vec3(d1))), 1.0);
        float depth_result = min(d0, d1);

        vec4 col_result2 = vec4(mix(col_result.rgb, c2.rgb * layer[2].alpha, step(vec3(d2), vec3(depth_result))), 1.0);
        vec4 position_result2 = vec4(mix(position_result.rgb, p2.rgb, step(vec3(d2), vec3(depth_result))), 1.0);
        float depth_result2 = min(d2, depth_result);

        vec4 col_result3 = vec4(mix(col_result2.rgb, c3.rgb * layer[3].alpha, step(vec3(d3), vec3(depth_result2))), 1.0);
        vec4 position_result3 = vec4(mix(position_result2.rgb, p3.rgb, step(vec3(d3), vec3(depth_result2))), 1.0);
        float depth_result3 = min(depth_result2, d3);

        fragColor_color = vec4(mix(col_result3.rgb, c4.rgb * layer[4].alpha, step(vec3(d4), vec3(depth_result3))), 1.0);
        world = vec4(mix(position_result3.rgb, p4.rgb, step(vec3(d4), vec3(depth_result3))), 1.0);
        fragColor_position = world;
        fragColor_normal = vec4(calcFlatNormal(vec3(world.x, world.y, world.z)), 1.0);
        gl_FragDepth = min(depth_result3, d4);
    }
    else if(u_layer_num == 6) {
        float d0 = texture(layer[0].depth, v_texcoord).r;
        float d1 = texture(layer[1].depth, v_texcoord).r;
        float d2 = texture(layer[2].depth, v_texcoord).r;
        float d3 = texture(layer[3].depth, v_texcoord).r;
        float d4 = texture(layer[4].depth, v_texcoord).r;
        float d5 = texture(layer[5].depth, v_texcoord).r;
        
        vec4 c0  = texture(layer[0].color, v_texcoord);
        vec4 c1  = texture(layer[1].color, v_texcoord);
        vec4 c2  = texture(layer[2].color, v_texcoord);
        vec4 c3  = texture(layer[3].color, v_texcoord);
        vec4 c4  = texture(layer[4].color, v_texcoord);
        vec4 c5  = texture(layer[5].color, v_texcoord);

        vec4 p0  = texture(layer[0].position, v_texcoord);
        vec4 p1  = texture(layer[1].position, v_texcoord);
        vec4 p2  = texture(layer[2].position, v_texcoord);
        vec4 p3  = texture(layer[3].position, v_texcoord);
        vec4 p4  = texture(layer[4].position, v_texcoord);
        vec4 p5  = texture(layer[5].position, v_texcoord);

        vec4 world = vec4(0.0);

        vec4 col_result = vec4(mix(c1.rgb * layer[1].alpha, c0.rgb * layer[0].alpha, step(vec3(d0), vec3(d1))), 1.0);
        vec4 position_result = vec4(mix(p1.rgb, p0.rgb, step(vec3(d0), vec3(d1))), 1.0);
        float depth_result = min(d0, d1);

        vec4 col_result2 = vec4(mix(col_result.rgb, c2.rgb * layer[2].alpha, step(vec3(d2), vec3(depth_result))), 1.0);
        vec4 position_result2 = vec4(mix(position_result.rgb, p2.rgb, step(vec3(d2), vec3(depth_result))), 1.0);
        float depth_result2 = min(d2, depth_result);

        vec4 col_result3 = vec4(mix(col_result2.rgb, c3.rgb * layer[3].alpha, step(vec3(d3), vec3(depth_result2))), 1.0);
        vec4 position_result3 = vec4(mix(position_result2.rgb, p3.rgb, step(vec3(d3), vec3(depth_result2))), 1.0);
        float depth_result3 = min(depth_result2, d3);

        vec4 col_result4 = vec4(mix(col_result3.rgb, c4.rgb * layer[4].alpha, step(vec3(d4), vec3(depth_result3))), 1.0);
        vec4 position_result4 = vec4(mix(position_result3.rgb, p4.rgb, step(vec3(d4), vec3(depth_result3))), 1.0);
        float depth_result4 = min(depth_result3, d4);

        fragColor_color = vec4(mix(col_result4.rgb, c5.rgb * layer[5].alpha, step(vec3(d5), vec3(depth_result4))), 1.0);
        world = vec4(mix(position_result4.rgb, p5.rgb, step(vec3(d5), vec3(depth_result4))), 1.0);
        fragColor_position = world;
        fragColor_normal = vec4(calcFlatNormal(vec3(world.x, world.y, world.z)), 1.0);
        gl_FragDepth = min(depth_result4, d5);
    }
}
