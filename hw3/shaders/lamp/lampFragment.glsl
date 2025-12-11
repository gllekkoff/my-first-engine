#version 330 core
out vec4 FragColor;
uniform vec3 lampColor;
uniform float emissiveIntensity;
void main() {
    FragColor = vec4(lampColor * emissiveIntensity, 1.0);
}
