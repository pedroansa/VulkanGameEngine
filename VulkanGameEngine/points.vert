#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
    mat4 transform; // projection * view * model
} push;

void main() {
    gl_Position = push.transform * vec4(position, 1.0);
    
    // Para pontos, use cor s�lida ou cor do v�rtice
    fragColor = vec3(1.0, 1.0, 1.0); // Branco fixo para pontos
    
    // Tamanho dos pontos em pixels
    gl_PointSize = 6.0;
}