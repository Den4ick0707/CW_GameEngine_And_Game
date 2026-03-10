#version 330 core

// Наші атрибути (Layout), які ми налаштовували в C++
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

// Уніформи від нашого Рендерера
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

// Передаємо колір у фрагментний шейдер
out vec4 v_Color;

void main() {
    v_Color = a_Color;
    // Множимо позицію вершини на матриці об'єкта та камери
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}