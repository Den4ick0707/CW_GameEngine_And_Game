#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in  float v_TexIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[32];

void main()
{
    vec4 texColor = v_Color;

    switch(int(v_TexIndex))
    {
        case 0: texColor *= texture(u_Textures[0], v_TexCoord * v_TilingFactor); break;
        case 1: texColor *= texture(u_Textures[1], v_TexCoord * v_TilingFactor); break;
        case 2: texColor *= texture(u_Textures[2], v_TexCoord * v_TilingFactor); break;
        case 3: texColor *= texture(u_Textures[3], v_TexCoord * v_TilingFactor); break;
        case 4: texColor *= texture(u_Textures[4], v_TexCoord * v_TilingFactor); break;
        case 5: texColor *= texture(u_Textures[5], v_TexCoord * v_TilingFactor); break;
        case 6: texColor *= texture(u_Textures[6], v_TexCoord * v_TilingFactor); break;
        case 7: texColor *= texture(u_Textures[7], v_TexCoord * v_TilingFactor); break;
    }

    if(texColor.a < 0.01)
    discard;

    FragColor = texColor;
}