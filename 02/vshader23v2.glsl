#version 330 core

in vec3 aPosition;
in vec4 aColor;
out vec4 vColor;

void main() {
    vColor = aColor;
    gl_PointSize = 3.0;
    gl_Position = vec4(aPosition, 1.0);
}
