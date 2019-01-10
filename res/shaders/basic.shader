#shader vertex
#version 300 es

layout(location = 0) in vec4 position;

void main(void)
{
    gl_Position = position;
};

#shader fragment
#version 300 es

precision lowp float;
layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main(void)
{
    color = u_Color;
};
