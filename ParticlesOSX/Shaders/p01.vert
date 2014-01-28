#version 410

layout (location = 0) in vec2 VertexPosition;
layout (location = 1) in vec3 VertexColor;
layout (location = 2) in vec2 ParticleCenter;

out vec3 color;
out vec2 center;

uniform ivec2 viewSize;

void main()
{
    color = VertexColor;
    center = ParticleCenter;
    gl_Position = vec4(VertexPosition[0] / (viewSize[0] / 2), VertexPosition[1] / (viewSize[1] / 2), 0.0, 1.0);
}
