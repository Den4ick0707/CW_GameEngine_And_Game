#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 v_Color;
in vec2 v_TexCoord; // Якщо ви використовуєте текстури

void main()
{
    // 1. Базовий колір
    vec4 texColor = v_Color;

    // 2. Легкий ефект неонового світіння (робимо яскраві кольори ще яскравішими)
    // Використовуємо ступінь, щоб темні кольори (фон) залишились темними
    vec3 bloom = pow(texColor.rgb, vec3(2.2)) * 0.5;
    vec3 finalColor = texColor.rgb + bloom;

    // 3. Віньєтка (Vignette) - затемнення по краях
    // Отримуємо координати екрану від 0.0 до 1.0 (приблизно)
    vec2 screenUV = gl_FragCoord.xy / vec2(1280.0, 720.0); // Замініть на розмір вашого вікна
    vec2 centerUV = screenUV * 2.0 - 1.0; // Переводимо в діапазон [-1; 1]

    // Обчислюємо відстань від центру
    float dist = length(centerUV);

    // Smoothstep створює м'який градієнт на краях
    float vignette = smoothstep(1.5, 0.4, dist);

    finalColor *= vignette;

    // 4. Легкий ефект Scanlines (смуги старого телевізора)
    float scanline = sin(gl_FragCoord.y * 1.5) * 0.03;
    finalColor -= scanline;

    FragColor = vec4(finalColor, texColor.a);
}