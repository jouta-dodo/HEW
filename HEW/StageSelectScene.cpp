#include "StageSelectScene.h"
#include "GameScene.h"
#include "TitleScene.h"
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
    texGameReturn = Graphics::LoadTexture(L"assets\\Game_Modoru.png");
    texTitleReturn = Graphics::LoadTexture(L"assets\\Title_Modoru.png");
    saveData.Load();

    player.Init(texPlayer);
    player.SetPos(200.0f, 550.0f);
    cameraX = 0.0f;
    fWasPressed = false;
    menuOpen = false;
    menuSelection = 0;
    tabWasPressed = false;
    menuUpWasPressed = false;
    menuDownWasPressed = false;
    menuConfirmWasPressed = false;
}

void StageSelectScene::Update()
{
    const bool tabIsPressed =
        (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;

    if (tabIsPressed && !tabWasPressed)
    {
        menuOpen = !menuOpen;
    }

    tabWasPressed = tabIsPressed;

    if (menuOpen)
    {
        const bool upIsPressed =
            (GetAsyncKeyState('W') & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
        const bool downIsPressed =
            (GetAsyncKeyState('S') & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
        const bool confirmIsPressed =
            (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

        if (upIsPressed && !menuUpWasPressed)
        {
            menuSelection = (menuSelection + 1) % 2;
        }

        if (downIsPressed && !menuDownWasPressed)
        {
            menuSelection = (menuSelection + 1) % 2;
        }

        if (confirmIsPressed && !menuConfirmWasPressed)
        {
            if (menuSelection == 0)
            {
                menuOpen = false;
            }
            else
            {
                SceneManager::ChangeScene(new TitleScene());
                return;
            }
        }

        menuUpWasPressed = upIsPressed;
        menuDownWasPressed = downIsPressed;
        menuConfirmWasPressed = confirmIsPressed;
        return;
    }

    menuUpWasPressed = false;
    menuDownWasPressed = false;
    menuConfirmWasPressed = false;
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
            SceneManager::ChangeScene(new GameScene(i));
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

void StageSelectScene::DrawDigit(
    int digit,
    float x,
    float y,
    float size,
    const ColorF& color)
{
    static const bool segments[10][7] = {
        { true, true, true, true, true, true, false },
        { false, true, true, false, false, false, false },
        { true, true, false, true, true, false, true },
        { true, true, true, true, false, false, true },
        { false, true, true, false, false, true, true },
        { true, false, true, true, false, true, true },
        { true, false, true, true, true, true, true },
        { true, true, true, false, false, false, false },
        { true, true, true, true, true, true, true },
        { true, true, true, true, false, true, true }
    };

    if (digit < 0 || digit > 9) return;

    float thickness = size * 0.18f;
    float half = size * 0.5f;

    if (segments[digit][0]) Graphics::DrawRect(x + thickness, y, size, thickness, color);
    if (segments[digit][1]) Graphics::DrawRect(x + size, y + thickness, thickness, half, color);
    if (segments[digit][2]) Graphics::DrawRect(x + size, y + half, thickness, half, color);
    if (segments[digit][3]) Graphics::DrawRect(x + thickness, y + size, size, thickness, color);
    if (segments[digit][4]) Graphics::DrawRect(x, y + half, thickness, half, color);
    if (segments[digit][5]) Graphics::DrawRect(x, y + thickness, thickness, half, color);
    if (segments[digit][6]) Graphics::DrawRect(x + thickness, y + half - thickness * 0.5f, size, thickness, color);
}

void StageSelectScene::Draw()
{
    if (texWhite.Get() != nullptr)
    {
        int totalStars = 0;
        for (int i = 0; i < DoorCount; i++)
        {
            totalStars += saveData.GetStageData(i).earnedStars;
        }

        const float totalStarX = 1120.0f;
        const float totalStarY = 28.0f;
        const float totalDigitSize = 24.0f;

        DrawDigit(
            totalStars,
            totalStarX,
            totalStarY,
            totalDigitSize,
            Color::White);
        Graphics::DrawRect(
            totalStarX + 34.0f,
            totalStarY + 11.0f,
            18.0f,
            3.0f,
            Color::White);
        DrawDigit(
            DoorCount * 3,
            totalStarX + 60.0f,
            totalStarY,
            totalDigitSize,
            Color::White);

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

            const int earnedStars =
                saveData.GetStageData(i).earnedStars;
            const float starX = doorX[i] - cameraX + 28.0f;
            const float starY = doorY[i] - 42.0f;
            const float digitSize = 18.0f;

            DrawDigit(earnedStars, starX, starY, digitSize, Color::White);
            Graphics::DrawRect(
                starX + 27.0f,
                starY + 8.0f,
                14.0f,
                3.0f,
                Color::White);
            DrawDigit(3, starX + 47.0f, starY, digitSize, Color::White);
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

    if (menuOpen)
    {
        DrawPauseMenu();
    }
}

void StageSelectScene::DrawPauseMenu()
{
    Graphics::DrawRect(
        0.0f,
        0.0f,
        PuzzleConstants::WindowWidth,
        720.0f,
        ColorF(0.0f, 0.0f, 0.0f, 0.45f));

    const float menuX = 440.0f;
    const float menuY = 280.0f;
    const float menuWidth = 400.0f;
    const float menuHeight = 75.0f;
    const float menuSpacing = 35.0f;
    ComPtr<ID3D11ShaderResourceView> menuTextures[2] = {
        texGameReturn,
        texTitleReturn
    };

    for (int i = 0; i < 2; i++)
    {
        const float y = menuY + i * (menuHeight + menuSpacing);
        const float scale = (i == menuSelection) ? 1.15f : 1.0f;
        const float drawWidth = menuWidth * scale;
        const float drawHeight = menuHeight * scale;
        const float drawX = menuX + (menuWidth - drawWidth) * 0.5f;
        const float drawY = y + (menuHeight - drawHeight) * 0.5f;

        if (menuTextures[i].Get() != nullptr)
        {
            Graphics::DrawImage(
                drawX,
                drawY,
                drawWidth,
                drawHeight,
                menuTextures[i],
                Color::White);
        }
    }
}
