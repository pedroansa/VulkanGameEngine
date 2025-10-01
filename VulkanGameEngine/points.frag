#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    // Opcional: fazer pontos circulares em vez de quadrados
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    if (length(coord) > 1.0) {
        discard; // Descarta pixels fora do círculo
    }
    
    outColor = vec4(fragColor, 1.0);
}