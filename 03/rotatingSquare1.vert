#version 330

in vec4 aPosition;
uniform float uTheta;

void main()
{
    float s = sin(uTheta);
    float c = cos(uTheta);

    gl_Position.x = -s * aPosition.y + c * aPosition.x;
    gl_Position.y =  s * aPosition.x + c * aPosition.y;
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;
}
