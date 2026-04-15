#ifndef COURSE_WORK_DARYEV_GAME_COMPONENTS_H
#define COURSE_WORK_DARYEV_GAME_COMPONENTS_H


struct CarComponent {
    float Speed = 0.0f;
    float MaxSpeed = 30.0f;
    float Acceleration = 20.0f;
    float TurnSpeed = 250.0f;
    float Drag = 5.0f;

    int CurrentLap = 0;
    float BestLapTime = 999.0f;
    float CurrentLapTime = 0.0f;
    bool PassedHalfway = false;
};

struct TrackWallComponent {
    float Bounciness = 0.6f;
};

struct CheckpointComponent {
    bool IsFinishLine = false;
    bool IsHalfwayPoint = false;
};

struct SkidMarkComponent {
    float LifeTime = 2.0f;
    float MaxLife = 2.0f;
};



#endif //COURSE_WORK_DARYEV_GAME_COMPONENTS_H