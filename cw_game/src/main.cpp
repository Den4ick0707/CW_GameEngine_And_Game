#include "engine.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "scene.h"
#include "scene_manager.h"
#include "entry_point.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <json.hpp>
#include <iostream>

#include "game_components.h"
#include "platformer_scene.h"

using json = nlohmann::json;
using namespace Engine::Core;
using namespace Engine::Scene;
using namespace Engine::Graphics;


class GameApp : public Engine::Core::Application {
public:
    GameApp() {
        RenderCommand::Init();
        Renderer2D::Init();
        Input::Get().BindAction("Exit", KeyCode::Escape);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        ImGui::StyleColorsDark();
        GLFWwindow *window = static_cast<GLFWwindow *>(GetWindow().GetNativeWindow());
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        auto scene = std::make_shared<PlatformerScene>();
        SceneManager::Add("Platformer", scene);
        SceneManager::LoadScene("Platformer");
    }

    ~GameApp() override {
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

        auto *pc = dynamic_cast<PlatformerScene *>(SceneManager::GetActive());

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.1f", Time::GetFPS());
        ImGui::Text("Controls:");
        ImGui::BulletText("A/D or Arrows: Move");
        ImGui::BulletText("Space/W: Jump (Hold to jump higher)");
        ImGui::BulletText("C or Shift: DASH (Air dash resets on floor/walls)");
        ImGui::Separator();

        if (pc && pc->GetRegistry().Has<PlayerComponent>(pc->GetPlayer())) {
            auto &pComp = pc->GetRegistry().Get<PlayerComponent>(pc->GetPlayer());
            ImGui::Text("Dash Ready: %s", pComp.HasDash ? "YES" : "NO");
            ImGui::Text("Wall Sliding: %s", pComp.IsWallSliding ? "YES" : "NO");
        }

        if (ImGui::Button("Restart Level")) {
            if (pc) {
                pc->Restart();
            }
        }
        ImGui::End();

        if (pc && pc->IsGameWon()) {
            ImGui::SetNextWindowPos(ImVec2(GetWindow().GetWidth() * 0.5f, GetWindow().GetHeight() * 0.5f),
                                    ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::Begin("WIN", nullptr,
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav);
            ImGui::SetWindowFontScale(3.0f);
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.8f, 1.0f), "TOWER CONQUERED!");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Text("You've mastered the Dash and Wall Jumps.");
            ImGui::End();
        }

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Renderer2D::ResetStats();
    }
};

Engine::Core::Application *Engine::Core::CreateApplication() { return new GameApp(); }
