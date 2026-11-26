#pragma once
#include "Scene.h"
#include "Player.h"
#include "Graphics.h"
#include "SaveData.h"
#include "Input.h"

class StageSelectScene : public IScene
{
private:
    Player player;
    ComPtr<ID3D11ShaderResourceView> texPlayer;
    ComPtr<ID3D11ShaderResourceView> texWhite;
    ComPtr<ID3D11ShaderResourceView> texFKey;

    static constexpr int DoorCount = 3;
    float doorX[DoorCount] = { 300.0f, 1100.0f, 1900.0f };
    float doorY[DoorCount] = { 550.0f, 550.0f, 550.0f };
    float doorWidth = 80.0f;
    float doorHeight = 100.0f;
    float cameraX = 0.0f;
    bool fWasPressed = false;
    bool playerNearDoor = false;
    SaveData saveData;
    ComPtr<ID3D11ShaderResourceView> texGameReturn;
    ComPtr<ID3D11ShaderResourceView> texTitleReturn;
    bool menuOpen = false;
    int menuSelection = 0;
    bool tabWasPressed = false;
    bool menuUpWasPressed = false;
    bool menuDownWasPressed = false;
    bool menuConfirmWasPressed = false;

    void DrawDigit(int digit, float x, float y, float size, const ColorF& color);
    void DrawPauseMenu();

    bool IsOverlapping(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

public:
    void Init() override;
    void Update() override;
    void Draw() override;
};