#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 Tex;
} vs;

uniform mat4 model, view, projection;

void main() {
    vec4 world = model * vec4(aPos, 1.0);
    vs.FragPos = world.xyz;
    vs.Normal  = mat3(transpose(inverse(model))) * aNormal;
    vs.Tex     = aTex;
    gl_Position = projection * view * world;
}
