#version 330

in vec4 aPosition;

void main()
{
    gl_PointSize = 1.0;
    gl_Position = aPosition;
}
