#version 430
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 TexCoord;
out vec2 f_TexCoord;
void main()
{
    f_TexCoord = TexCoord;
    gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
}