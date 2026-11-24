#pragma once
#include "Scene.h"
#include "Player.h"
#include "Graphics.h"

class StageSelectScene : public IScene
{
private:
    Player player;
    ComPtr<ID3D11ShaderResourceView> texPlayer;
    ComPtr<ID3D11ShaderResourceView> texWhite;

    static constexpr int DoorCount = 4;
    float doorX[DoorCount] = { 180.0f, 450.0f, 720.0f, 1000.0f };
    float doorY[DoorCount] = { 350.0f, 350.0f, 350.0f, 350.0f };
    float doorWidth = 80.0f;
    float doorHeight = 100.0f;
    bool fWasPressed = false;

    bool IsOverlapping(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

public:
    void Init() override;
    void Update() override;
    void Draw() override;
};