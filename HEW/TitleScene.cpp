#include "TitleScene.h"
#include "StageSelectScene.h"
#include "PuzzleConstants.h" 
#include <windows.h>
#include <iostream>

void TitleScene::Init()
{


    //フォルダの中の背景画像
    texBackground = Graphics::LoadTexture(L"assets\\Title_01.png");

}

void TitleScene::Update()
{
    // スペースキーが押されたら「ステージセレクト」に移動
    if ((GetAsyncKeyState(VK_SPACE) & 0x8000))
    {
        SceneManager::ChangeScene(new StageSelectScene());
    }
}

void TitleScene::Draw()
{
    //背景画像がを画面いっぱいに描画する
    if (texBackground.Get() != nullptr)
    {
        //画面の左上(0, 0)から、ウィンドウの幅・高さに合わせて描画
        Graphics::DrawImage(
            0.0f, 0.0f,
            PuzzleConstants::WindowWidth, 720.0f, //ここで画面サイズに合わせて調整できるぜ
            texBackground,
            Color::White
        );
    }

    
}