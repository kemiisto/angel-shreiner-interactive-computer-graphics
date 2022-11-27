#version 330

in vec4 aPosition;
in vec4 aColor;

out vec4 vColor;

void main()
{
    gl_Position = aPosition;
    vColor = aColor;
    gl_PointSize = 10.0;
}
