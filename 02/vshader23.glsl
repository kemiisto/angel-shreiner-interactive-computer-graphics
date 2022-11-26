#version 330 core

in vec4 vertexPosition;
out vec4 vertexColor;

void main() {
    vertexColor = vec4((1.0 + vertexPosition.xyz) / 2.0, 1.0);
    gl_PointSize = 3.0;
    gl_Position = vertexPosition;
}
