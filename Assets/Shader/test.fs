#version 430
in vec2 f_TexCoord;
out vec4 FragColor;
uniform sampler2D u_texture;
void main()
{
    FragColor = texture(u_texture,f_TexCoord);
}