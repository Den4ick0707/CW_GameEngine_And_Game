#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;
// u_Transform повністю прибрали!

out vec4 v_Color;

void main() {
    v_Color = a_Color;
    // Тепер множимо тільки на матрицю камери
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}