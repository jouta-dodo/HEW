#include "StageSelectScene.h"
#include "GameScene.h" // パズルゲーム本体へ遷移するため
#include "PuzzleConstants.h"
#include <windows.h>
#include <iostream>

bool StageSelectScene::IsOverlapping(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
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
    // プレイヤーの移動と重力を更新
    player.Update(PuzzleConstants::WindowWidth, 720.0f);

    // 扉の近くにいるか判定
    bool isNearDoor = IsOverlapping(
        player.GetX(), player.GetY(), player.GetWidth(), player.GetHeight(),
        doorX, doorY, doorWidth, doorHeight
    );

    // 扉近くで「Fキー」を押したら「パズルゲーム本体」に移動
    if (isNearDoor && (GetAsyncKeyState('F') & 0x8000))
    {
        SceneManager::ChangeScene(new GameScene());
    }
}

void StageSelectScene::Draw()
{
    if (texWhite.Get() != nullptr)
    {
        // 簡単な床の描画 (Colorf -> ColorF に修正)
        Graphics::DrawImage(100.0f, 450.0f, 1080.0f, 20.0f, texWhite, ColorF(0.3f, 0.3f, 0.3f, 1.0f));
        // 赤色の扉の描画
        Graphics::DrawImage(doorX, doorY, doorWidth, doorHeight, texWhite, Color::Red);
    }

    // プレイヤーの描画
    player.Draw();
}