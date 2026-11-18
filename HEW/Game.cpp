#include "Game.h"
#include "Input.h"
#include "Scene.h"
#include "TitleScene.h"
#include <iostream>

using namespace std;

//=================================================================================
//使い方★
//namespace Gameの中で必ず書く
// 前のやつと同じようにInit(初期化),Update(更新),Draw(描画)の三種の神器を使います
//例 タイトルシーン
//class TitleScene:public IScene
//{
//void Init()override<-これ絶対書いてね
//	{
//		std::cout<<"おなかすいた";
//	}
//void Update()override
//	{
//		
//	}
//void Draw()override
//	{
//
//	}
//上みたいな感じね
//大元のゲームシーンは三種の神器直接使えばオッケー
//=================================================================================
namespace Game
{
	enum class Cmd { Jump, MoveLeft, MoveRight };

	// フレームワークが大元のループから呼び出す関数
	void Init()
	{
		Input::BindAction(Cmd::MoveLeft, { Key::Left, Key::A }, { Button::Left });
		Input::BindAction(Cmd::MoveRight, { Key::Right, Key::D }, { Button::Right });
		cout << "初期化完了\n";

		// 最初に起動するシーンとして「タイトル画面」を設定
		SceneManager::ChangeScene(new TitleScene());
	}

	void Update()
	{
		// 現在アクティブなシーンの更新処理を呼ぶ
		SceneManager::Update();
	}

	void Draw()
	{
		// 現在アクティブなシーンの描画処理を呼ぶ
		SceneManager::Draw();
	}
}