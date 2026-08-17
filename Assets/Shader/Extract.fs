#version 430
out vec4 FragColor;
in vec2 f_TexCoord;
uniform sampler2D u_scene;
void main()
{
    vec4 color = texture(u_scene,f_TexCoord);
    if(color.a > 0.99 && color.a < 1.0)
    {
        FragColor = vec4(color.rgb,1.0); 
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}