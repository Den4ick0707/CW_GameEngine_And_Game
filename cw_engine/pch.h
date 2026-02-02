#ifndef COURSEWORK_GAMEENGINE_PCH_H
#define COURSEWORK_GAMEENGINE_PCH_H

#ifndef PCH_H
#define PCH_H

// Стандартні бібліотеки
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <fstream>


// Бібліотеки рушія (ті, що ти додав)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <../stb/stb_image.h> // Переконайся, що шлях правильний відносно include directories
// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#endif //PCH_H


#endif //COURSEWORK_GAMEENGINE_PCH_H
