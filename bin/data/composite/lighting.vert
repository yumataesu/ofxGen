#version 410
uniform mat4 view;

in vec3 position;
in vec2 texcoord;

out vec2 v_texcoord;
out vec3 v_light_pos;

void main() {
    v_texcoord = vec2(texcoord.x, texcoord.y);
    gl_Position =   vec4(position, 1.0);
}
