#version 330

in vec3 aPosition;
in vec3 aColor;
out vec4 vColor;

void main()
{
    gl_Position = vec4(aPosition, 1.0);
    vColor = vec4(aColor, 1.0);
}
