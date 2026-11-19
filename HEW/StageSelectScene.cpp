#include "StageSelectScene.h"
#include "GameScene.h"
#include "PuzzleConstants.h"

bool StageSelectScene::IsOverlapping(
    float x1,
    float y1,
    float w1,
    float h1,
    float x2,
    float y2,
    float w2,
    float h2)
{
    return x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2;
}

void StageSelectScene::Init()
{
    std::cout << "=== ステージセレクト初期化 ===\n";

    texPlayer = Graphics::LoadTexture(L"assets\\ui.png");
    texWhite = Graphics::LoadTexture(L"assets\\siroiro.png");

    player.Init(texPlayer);
    player.SetPos(200.0f, 350.0f);
}

void StageSelectScene::Update()
{
    player.Update(PuzzleConstants::WindowWidth, 720.0f);

    const bool isNearDoor = IsOverlapping(
        player.GetX(),
        player.GetY(),
        player.GetWidth(),
        player.GetHeight(),
        doorX,
        doorY,
        doorWidth,
        doorHeight
    );

    // 扉に入るとゲーム本編へ移動
    if (isNearDoor)
    {
        SceneManager::ChangeScene(new GameScene());
    }
}

void StageSelectScene::Draw()
{
    if (texWhite.Get() != nullptr)
    {
        Graphics::DrawImage(
            100.0f,
            450.0f,
            1080.0f,
            20.0f,
            texWhite,
            ColorF(0.3f, 0.3f, 0.3f, 1.0f)
        );

        Graphics::DrawImage(
            doorX,
            doorY,
            doorWidth,
            doorHeight,
            texWhite,
            Color::Red
        );
    }

    player.Draw();
}