#version 330 core

in vec3 aPosition;
in vec3 aColor;
out vec4 vColor;

void main() {
    vColor = vec4(aColor, 1.0);
    gl_Position = vec4(aPosition, 1.0);
}
