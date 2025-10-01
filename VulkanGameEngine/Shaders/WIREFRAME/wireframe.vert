#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
    mat4 transform; // projection * view * model
} push;

void main() {
    gl_Position = push.transform * vec4(position, 1.0);
    
    // Para wireframe, use a cor original do vértice ou cor fixa
    fragColor = color; 
    }