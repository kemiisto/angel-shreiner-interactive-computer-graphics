#version 330 core

in vec3 aPosition;
out vec4 vColor;

void main() {
    vColor = vec4((1.0 + aPosition) / 2.0, 1.0);
    gl_PointSize = 3.0;
    gl_Position = vec4(aPosition, 1.0);
}