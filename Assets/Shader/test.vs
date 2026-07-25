#version 430
layout (location = 0) in vec2 position;
layout (location = 0) in vec2 TexCoord;
out vec2 f_TexCoord;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
void main()
{
    f_TexCoord = TexCoord;
    gl_Position = u_projection * u_view * u_model * vec4(position,0.0f,1.0f);
}