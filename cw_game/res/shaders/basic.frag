#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in float v_TexIndex;

void main()
{
    vec2 resolution = vec2(1920.0, 1080.0);

    vec4 texColor = v_Color;

    if(texColor.a < 0.01)
    discard;

    vec3 bloom = pow(texColor.rgb, vec3(2.2)) * 0.6;
    vec3 finalColor = texColor.rgb + bloom;

    // 3. Віньєтка (Vignette) - затемнення країв екрана
    vec2 screenUV = gl_FragCoord.xy / resolution;
    vec2 centerUV = screenUV * 2.0 - 1.0;

    // Обчислюємо відстань від центру
    float dist = length(centerUV);

    // Створюємо м'яке затемнення
    float vignette = smoothstep(1.2, 0.4, dist);
    finalColor *= vignette;

    // 4. Ефект Scanlines (смуги старого телевізора)
    float scanline = sin(gl_FragCoord.y * 2.0) * 0.04;
    finalColor -= scanline;

    FragColor = vec4(finalColor, texColor.a);
}