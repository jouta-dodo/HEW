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

    float doorX = 1000.0f;
    float doorY = 350.0f;
    float doorWidth = 80.0f;
    float doorHeight = 100.0f;

    bool IsOverlapping(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

public:
    void Init() override;
    void Update() override;
    void Draw() override;
};