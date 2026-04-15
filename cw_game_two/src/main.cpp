#include "engine.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "scene.h"
#include "scene_manager.h"
#include "entry_point.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <json.hpp>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "game_components.h"
#include "racing_scene.h"

using json = nlohmann::json;
using namespace Engine::Core;
using namespace Engine::Scene;
using namespace Engine::Graphics;


class RacingApp : public Engine::Core::Application {
public:
    RacingApp() {
        RenderCommand::Init();
        Renderer2D::Init();
        Input::Get().BindAction("Exit", KeyCode::Escape);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        GLFWwindow *window = static_cast<GLFWwindow *>(GetWindow().GetNativeWindow());
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        auto scene = std::make_shared<RacingScene>();
        SceneManager::Add("Racing", scene);
        SceneManager::LoadScene("Racing");
    }

    ~RacingApp() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        Renderer2D::Shutdown();
        SceneManager::Clear();
    }

    void Update(float dt) override {
        if (auto *scene = SceneManager::GetActive()) scene->Update(dt);
    }

    void Render() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (auto *scene = SceneManager::GetActive()) scene->OnRender();

        auto *rc = dynamic_cast<RacingScene *>(SceneManager::GetActive());

        ImGui::Begin("Dashboard", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
        ImGui::SetWindowPos(ImVec2(10, 10), ImGuiCond_Always);

        ImGui::Text("FPS: %.1f", Time::GetFPS());
        ImGui::Separator();

        if (rc && rc->GetRegistry().Has<CarComponent>(rc->GetCar())) {
            auto &car = rc->GetRegistry().Get<CarComponent>(rc->GetCar());

            ImGui::Text("Speed: %.1f km/h", std::abs(car.Speed) * 5.0f);
            ImGui::ProgressBar(std::abs(car.Speed) / car.MaxSpeed, ImVec2(200.0f, 0.0f));

            ImGui::Separator();
            ImGui::Text("Lap: %d", car.CurrentLap);
            ImGui::Text("Current Time: %.2f s", car.CurrentLapTime);
            if (car.BestLapTime < 900.0f) {
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Best Time: %.2f s", car.BestLapTime);
            }
            ImGui::Text("Halfway passed: %s", car.PassedHalfway ? "Yes" : "No");

            ImGui::Separator();
            if (ImGui::Button("Restart Position")) {
                rc->Restart();
            }
        }


        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Renderer2D::ResetStats();
    }
};

Engine::Core::Application *Engine::Core::CreateApplication() { return new RacingApp(); }