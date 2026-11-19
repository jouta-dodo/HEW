#pragma once
#include "Scene.h"
#include "Player.h"
#include "CSVLoad.h"
#include "BlockOperation.h"
#include "PlacementSimulator.h"
#include "CountJudgeMock.h"
#include "PuzzleConstants.h"
#include "Graphics.h"
#include "Input.h"

class GameScene : public IScene
{
private:
    ComPtr<ID3D11ShaderResourceView> texPlayer;
    ComPtr<ID3D11ShaderResourceView> texWhite;
    Player player;

    CSVLoad blockdata;
    BlockOperation bo;

    bool rWasPressed = false;
    GamePhase currentPhase = GamePhase::Setup;
    PlacementSimulator simulator;
    CountJudgeMock judgeSystem;
    int currentTick = 1;

    float tickTimer = 0.0f;
    const float tickInterval = 0.5f;

    bool needsRedraw = true;
    bool isSpacePressedLastFrame = false;

    bool stageStarCollected = false;
    bool clearStarCollected = false;
    bool tickStarCollected = false;

    bool resultShown = false;

    int tickLoopCount = 1;

    bool IsOverlapping(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
    void DrawDigit(int digit, float x, float y, float size, const ColorF& color);
    void DrawStarCount();
    void DrawResultOverlay();
    //void OnStageCleared();

public:
    void Init() override;
    void Update() override;
    void Draw() override;
};