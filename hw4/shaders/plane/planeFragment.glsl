#version 330 core
out vec4 FragColor;

in vec3 WorldPos;
in vec2 TexCoord;

uniform sampler2D floorTexture;

void main()
{
    vec2 tiled_coords = WorldPos.xz / 10.0;
    FragColor = texture(floorTexture, tiled_coords);
}