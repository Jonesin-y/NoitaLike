#version 430
out vec4 FragColor;
in vec2 f_TexCoord;
uniform sampler2D u_scene;
uniform sampler2D u_bloomBlur;
void main()
{
    vec3 backgroundColor = vec3(0.1f);
    float bloomStreangth = 0.8;
    vec4 SceneColor = texture(u_scene, f_TexCoord);
    vec4 bloomColor = texture(u_bloomBlur,f_TexCoord);
    vec3 sceneOnBackground = mix(backgroundColor,SceneColor.rgb,SceneColor.a);
    vec3 result = sceneOnBackground + bloomColor.rgb * bloomStreangth; // simple linear combination
    FragColor = vec4(result, 1.0);
}