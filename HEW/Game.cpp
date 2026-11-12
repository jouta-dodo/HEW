#include "Game.h"
#include"Input.h"
#include"Graphics.h"
#include"Scene.h"
#include<iostream>

using namespace std;

//=================================================================================
//使い方★
//namespace Gameの中に必ず書く
// 前のやつと同じようにInit(初期化),Update(更新),Draw(描画)の三種の神器を使います
// ISceneを新しく作成するシーンに継承します
//例　タイトルシーン
//class TitleScene:public IScene
//{
//void Init()overrride<-これ絶対書いてね	初期化
//	{
//		std::cout<<"おなかすいた";
//	{
//void Update()override　	更新処理
//	{
//		
//	}
//void Draw()override	描画処理
//	{
//
//	}
//上みたいな感じね
//大元のゲームシーンは三種の神器直接使えばオッケー
//=================================================================================
namespace Game
{
	enum class Cmd { Jump, MoveLeft, MoveRight };
	ComPtr<ID3D11ShaderResourceView> texPlayer;

	float playerX = 640.0f - 50.0f;
	float playerY = 360.0f - 50.0f;

	//=======
	//初期化
	//=======
	void Init()
	{
		//========================================================================================================================================
		// ActionBindの書き方
		// Input::BindAction(Cmd::コマンド名,{Key::キーの名前(キーの名前はInput.cpp参照),{Button::ボタンの名前(キーと同じくInput.cpp参照))})}
		// 使い方これだけ簡単
		//========================================================================================================================================
		
		//バインド処理
		Input::BindAction(Cmd::MoveLeft, { Key::Left,Key::A }, { Button::Left });
		Input::BindAction(Cmd::MoveRight, { Key::Right, Key::D }, { Button::Right });
		cout << "初期化完了"<<endl;

		//===========================================
		//ここにシーンの切り替え書く
		//SceneManager::ChangeScene(new TitleScene);
		//===========================================

		//==============
		//画像貼り付け
		//==============
		texPlayer = Graphics::LoadTexture(L"assets\\ui.png");

		if (texPlayer.Get() == nullptr)
		{
			cout << "エラー：画像の読み込み失敗";
		}
		else
		{
			cout << "画像読み込み成功" << endl;;
		}
	}

	//=========
	//更新処理
	//=========
	void Update()
	{
		if (Input::IsHeld(Cmd::MoveLeft)) playerX -= 5.0f;
		if (Input::IsHeld(Cmd::MoveRight)) playerX += 5.0f;
	}

	void Draw()
	{
		//Graphics::DrawRect(playerX, playerY, 100.0f, 100.0f, Color::Red);
		Graphics::DrawImage(playerX, playerY, 100.0f, 100.0f, texPlayer,Color::White);
	}

}