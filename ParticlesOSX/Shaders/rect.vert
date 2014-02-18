#version 410

layout (location = 0) in vec2 VertexPosition;
layout (location = 1) in vec4 VertexColor;
out vec4 color;

uniform vec2 viewSize;

void main()
{
    color = VertexColor;
    gl_Position = vec4(
                       VertexPosition[0] / (viewSize[0] / 2) - 1,
                       -VertexPosition[1] / (viewSize[1] / 2) +  1,
                       0.0,
                       1.0);
}
