#include "TitleScene.h"
#include "StageSelectScene.h"
#include "PuzzleConstants.h"
#include <windows.h>
#include <iostream>

void TitleScene::Init()
{
    texLogo = Graphics::LoadTexture(L"assets\\money_toushi_oinori.png");
    texStart = Graphics::LoadTexture(L"assets\\hajimekara.png");
    texContinue = Graphics::LoadTexture(L"assets\\tsuzukikara.png");
    texEnd = Graphics::LoadTexture(L"assets\\game_end.png");
    selectedOption = 0;
    // 前の画面で決定に使ったSpaceキーを離すまで、タイトル側では受け付けない。
    spaceWasPressed = true;
    upWasPressed = false;
    downWasPressed = false;
    enterWasPressed = false;
}

void TitleScene::Update()
{
    const bool spaceIsPressed =
        (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    const bool enterIsPressed =
        (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;
    const bool upIsPressed =
        (GetAsyncKeyState('W') & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
    const bool downIsPressed =
        (GetAsyncKeyState('S') & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;

    if (upIsPressed && !upWasPressed)
    {
        selectedOption = (selectedOption + 2) % 3;
    }

    if (downIsPressed && !downWasPressed)
    {
        selectedOption = (selectedOption + 1) % 3;
    }

    const bool decideIsPressed =
        (spaceIsPressed && !spaceWasPressed) ||
        (enterIsPressed && !enterWasPressed);

    if (decideIsPressed)
    {
        if (selectedOption == 2)
        {
            PostQuitMessage(0);
        }
        else if (selectedOption == 0)
        {
            SaveData saveData;
            saveData.Load();
            saveData.ResetAll();
            saveData.Save();
            SceneManager::ChangeScene(new StageSelectScene());
        }
        else
        {
            SceneManager::ChangeScene(new StageSelectScene());
        }
    }

    spaceWasPressed = spaceIsPressed;
    enterWasPressed = enterIsPressed;
    upWasPressed = upIsPressed;
    downWasPressed = downIsPressed;
}

void TitleScene::Draw()
{
    Graphics::DrawRect(
        0.0f,
        0.0f,
        PuzzleConstants::WindowWidth,
        720.0f,
        Color::White);

    if (texLogo.Get() != nullptr)
    {
        Graphics::DrawImage(
            390.0f,
            70.0f,
            500.0f,
            130.0f,
            texLogo,
            Color::White
        );
    }

    const float menuX = 440.0f;
    const float menuY = 280.0f;
    const float menuWidth = 400.0f;
    const float menuHeight = 70.0f;
    const float menuSpacing = 35.0f;

    ComPtr<ID3D11ShaderResourceView> menuTextures[3] = {
        texStart,
        texContinue,
        texEnd
    };

    for (int i = 0; i < 3; i++)
    {
        const float y = menuY +
            i * (menuHeight + menuSpacing);

        const float scale = (i == selectedOption) ? 1.15f : 1.0f;
        const float drawWidth = menuWidth * scale;
        const float drawHeight = menuHeight * scale;
        const float drawX =
            menuX + (menuWidth - drawWidth) * 0.5f;
        const float drawY =
            y + (menuHeight - drawHeight) * 0.5f;

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