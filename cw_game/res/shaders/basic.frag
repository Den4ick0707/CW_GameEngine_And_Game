#version 410 core

// ── Вхідні дані з вершинного шейдера ─────────────────────────────────────────
in vec4  v_Color;
in vec2  v_TexCoord;
in float v_TexIndex;

// ── Вихід ─────────────────────────────────────────────────────────────────────
layout(location = 0) out vec4 o_Color;

// ── Текстурні слоти (32 — максимум для більшості GPU) ─────────────────────────
uniform sampler2D u_Textures[32];

void main() {
    // OpenGL 4.1 дозволяє динамічне індексування масиву samplers
    o_Color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color;
}