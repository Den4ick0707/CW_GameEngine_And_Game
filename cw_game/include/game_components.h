#ifndef COURSE_WORK_DARYEV_GAME_COMPONENTS_H
#define COURSE_WORK_DARYEV_GAME_COMPONENTS_H

struct PlatformComponent {
    float Width = 2.0f;
    float Height = 0.3f;
};

struct CoinComponent {
    float Offset = 0.0f;
    float Speed = 2.0f;
    bool Collected = false;
};

struct ParallaxLayerComponent {
    float ScrollSpeed = 0.5f;
    float OriginalX = 0.0f;
};

struct SpikeComponent {
    bool Lethal = true;
};

struct BouncerComponent {
    float BounceForce = 28.0f;
};

struct WinZoneComponent {
    bool Reached = false;
};

struct PlayerComponent {
    float MoveSpeed = 12.0f;
    float JumpForce = 21.0f;
    int CoinsCollected = 0;

    float DashTimer = 0.0f;
    float DashDuration = 0.15f;
    float DashSpeed = 30.0f;
    float FacingDir = 1.0f;
    float WallDir = 0.0f;

    bool TouchingWall = false;
    bool IsWallSliding = false;
    bool OnGround = false;

    bool HasDash = true;
    bool IsDashing = false;

    bool SpaceWasPressed = false;
    bool DashWasPressed = false;
};






#endif //COURSE_WORK_DARYEV_GAME_COMPONENTS_H