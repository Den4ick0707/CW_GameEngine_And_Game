#ifndef COURSE_WORK_DARYEV_RACING_SCENE_H
#define COURSE_WORK_DARYEV_RACING_SCENE_H

#include "scene.h"
#include "components.h"

#include <json.hpp>

using json = nlohmann::json;
using namespace Engine::Core;
using namespace Engine::Scene;
using namespace Engine::Graphics;


class RacingScene : public Scene {
public:
    RacingScene();

    void Update(float dt) override;

    void OnRender() override;

    Entity GetCar() const;

    void Restart();

private:
    Entity m_Car = {NULL_ENTITY};
    bool m_ShouldQuit = false;

    void PrewarmSkidMarks();

    void SpawnSkidMark(glm::vec3 pos);

    void LoadTrackFromJson(const std::string &filepath);

    void SpawnFallbackTrack();

    void SpawnCar();

    void UpdateCar(float dt);

    void UpdateSkidMarks(float dt);

    void UpdateTransforms();

    void UpdateCollisions();

    void UpdateCamera(float dt);
};

#endif //COURSE_WORK_DARYEV_RACING_SCENE_H
