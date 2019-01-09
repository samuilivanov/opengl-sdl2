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

void main(void)
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
};
