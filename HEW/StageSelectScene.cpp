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
    texFKey = Graphics::LoadTexture(L"assets\\F_key_01.png");

    player.Init(texPlayer);
    player.SetPos(200.0f, 550.0f);
    cameraX = 0.0f;
    fWasPressed = false;
}

void StageSelectScene::Update()
{
    player.Update(PuzzleConstants::WorldWidth, 720.0f);

    cameraX = player.GetX() -
        PuzzleConstants::WindowWidth * 0.5f;

    if (cameraX < 0.0f)
    {
        cameraX = 0.0f;
    }

    const float maxCameraX =
        PuzzleConstants::WorldWidth -
        PuzzleConstants::WindowWidth;

    if (cameraX > maxCameraX)
    {
        cameraX = maxCameraX;
    }

    const bool fIsPressed =
        (GetAsyncKeyState('F') & 0x8000) != 0;

    playerNearDoor = false;

    for (int i = 0; i < DoorCount; i++)
    {
        const bool isNearDoor = IsOverlapping(
            player.GetX(),
            player.GetY(),
            player.GetWidth(),
            player.GetHeight(),
            doorX[i] - 30.0f,
            doorY[i] - 30.0f,
            doorWidth + 60.0f,
            doorHeight + 60.0f
        );

        if (isNearDoor && fIsPressed && !fWasPressed)
        {
            SceneManager::ChangeScene(new GameScene());
            fWasPressed = fIsPressed;
            return;
        }

        if (isNearDoor)
        {
            playerNearDoor = true;
        }
    }

    fWasPressed = fIsPressed;
}

void StageSelectScene::Draw()
{
    if (texWhite.Get() != nullptr)
    {
        Graphics::DrawImage(
            -cameraX,
            650.0f,
            PuzzleConstants::WorldWidth,
            70.0f,
            texWhite,
            ColorF(0.3f, 0.3f, 0.3f, 1.0f)
        );

        const ColorF doorColors[DoorCount] = {
            Color::Red,
            Color::Blue,
            Color::Green
        };

        for (int i = 0; i < DoorCount; i++)
        {
            Graphics::DrawImage(
                doorX[i] - cameraX,
                doorY[i],
                doorWidth,
                doorHeight,
                texWhite,
                doorColors[i]
            );
        }
    }

    player.Draw(cameraX);

    if (playerNearDoor && texFKey.Get() != nullptr)
    {
        const float promptSize = 48.0f;
        Graphics::DrawImage(
            player.GetX() - cameraX +
                (player.GetWidth() - promptSize) * 0.5f,
            player.GetY() - promptSize - 6.0f,
            promptSize,
            promptSize,
            texFKey,
            Color::White);
    }
}
