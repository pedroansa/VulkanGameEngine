#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionViewMatrix;
  vec3 directionToLight;
} ubo;

layout(push_constant) uniform Push{
	mat4 modelMatrix; 
	mat4 normalMatrix;
} push;

void main() {
    gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);    
    // Para pontos, use cor sólida ou cor do vértice
    fragColor = vec3(1.0, 1.0, 1.0); // Branco fixo para pontos
    
    // Tamanho dos pontos em pixels
    gl_PointSize = 2.0;
}