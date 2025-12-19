#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 Tex;
    vec4 FragPosLightSpace;
    mat3 TBN;
} vs_out;

uniform mat4 model, view, projection;
uniform mat4 lightSpaceMatrix;

void main() {
    vec4 world = model * vec4(aPos, 1.0);
    vs_out.FragPos = world.xyz;
    vs_out.Tex = aTex;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    vs_out.TBN = mat3(T, B, N);
    vs_out.Normal = N;

    gl_Position = projection * view * world;
}
