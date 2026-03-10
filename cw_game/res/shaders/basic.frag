#version 330 core

layout(location = 0) out vec4 color;

// Отримуємо колір з вершинного шейдера
in vec4 v_Color;

void main() {
    color = v_Color; // Просто малюємо піксель цим кольором
}