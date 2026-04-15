#ifndef COURSE_WORK_DARYEV_PLATFORMER_SCENE_H
#define COURSE_WORK_DARYEV_PLATFORMER_SCENE_H
#include <string>
#include "entity.h"
#include "scene.h"
#include "platformer_scene.h"

#include <json.hpp>
#include <iostream>


using json = nlohmann::json;

class PlatformerScene : public Engine::Scene::Scene {
public:
    PlatformerScene();

    void Update(float dt) override;

    void OnRender() override;


    Engine::Scene::Entity GetPlayer() const;

    bool IsGameWon() const;

    void Restart();

private:
    Engine::Scene::Entity m_Player = {Engine::Scene::NULL_ENTITY};
    bool m_ShouldQuit = false;
    bool m_GameWon = false;

    void AttachChild(Engine::Scene::Entity parent, Engine::Scene::Entity child);

    void LoadLevelFromJson(const std::string &filepath);

    void SpawnPlayer();

    void SpawnBackground();

    void UpdateHierarchy();

    void UpdateCoins(float time);

    void UpdatePlayer(float dt);

    void UpdatePhysics(float dt);

    void UpdateCollisions();

    void UpdateCamera(float dt);

    void UpdateParallax(float dt);
};


#endif //COURSE_WORK_DARYEV_PLATFORMER_SCENE_H
