#include "TitleScene.h"
#include "StageSelectScene.h"
#include "PuzzleConstants.h"
#include <windows.h>
#include <iostream>

void TitleScene::Init()
{
    texBackground = Graphics::LoadTexture(L"assets\\Title_01.png");
    spaceWasPressed = false;
}

void TitleScene::Update()
{
    const bool spaceIsPressed =
        (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    // スペースキーを押した瞬間だけステージセレクトへ移動
    if (spaceIsPressed && !spaceWasPressed)
    {
        SceneManager::ChangeScene(new StageSelectScene());
        return;
    }

    spaceWasPressed = spaceIsPressed;
}

void TitleScene::Draw()
{
    if (texBackground.Get() != nullptr)
    {
        Graphics::DrawImage(
            0.0f,
            0.0f,
            PuzzleConstants::WindowWidth,
            720.0f,
            texBackground,
            Color::White
        );
    }
}