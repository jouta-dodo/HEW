#include "Game.h"
#include "Input.h"
#include "Scene.h"
#include "TitleScene.h"
#include <iostream>

// パズル・判定モジュールおよび設定のインクルード
// ==========================================
#include "BlockData.h"
#include "PlacementSimulator.h"
#include "CountJudgeMock.h"
#include "PuzzleConstants.h" //統合した定数ファイル

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
	ComPtr<ID3D11ShaderResourceView> texPlayer;
	ComPtr<ID3D11ShaderResourceView> texWhite; //白の正方形画像 名前は　siroiro.png

	float playerX = 640.0f - 50.0f;
	float playerY = 360.0f - 50.0f;

	bool IsOverlapping(
		float x1,
		float y1,
		float width1,
		float height1,
		float x2,
		float y2,
		float width2,
		float height2)
	{
		return x1 < x2 + width2 &&
			x1 + width1 > x2 &&
			y1 < y2 + height2 &&
			y1 + height1 > y2;
	}

	// ==========================================
	// パズル・判定システムのインスタンスと変数
	// ==========================================
	GamePhase currentPhase = GamePhase::Setup;
	PlacementSimulator simulator;
	CountJudgeMock judgeSystem;
	int currentTick = 1;

	// ティック自動進行用のタイマー（1.5秒ごとに次の列へ）
	float tickTimer = 0.0f;
	const float tickInterval = 1.5f;

	// 画面のチカチカを防ぐための「描画更新フラグ」
	bool needsRedraw = true;

	// スペースキーの連続入力を防ぐための判定フラグ
	bool isSpacePressedLastFrame = false;

	// 星の収集状況
	bool stageStarCollected = false;
	bool puzzleStarCollected = false;

	void DrawDigit(int digit, float x, float y, float size, const ColorF& color)
	{
		//スターの数を表示するための７セグメント表示の描画関数 trueの線だけを描画する
		// 線の番号: 0=上、1=右上、2=右下、3=下、4=左下、5=左上、6=中央
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

		// 0〜9以外の数字は描画しない
		if (digit < 0 || digit > 9) return;

		// 数字の線の太さ・長さ・中央位置を計算
		float thickness = size * 0.18f;
		float length = size;
		float half = size * 0.5f;

		// trueになっている線だけを四角形として描画
		if (segments[digit][0]) Graphics::DrawRect(x + thickness, y, length, thickness, color);
		if (segments[digit][1]) Graphics::DrawRect(x + length, y + thickness, thickness, half, color);
		if (segments[digit][2]) Graphics::DrawRect(x + length, y + half, thickness, half, color);
		if (segments[digit][3]) Graphics::DrawRect(x + thickness, y + size, length, thickness, color);
		if (segments[digit][4]) Graphics::DrawRect(x, y + half, thickness, half, color);
		if (segments[digit][5]) Graphics::DrawRect(x, y + thickness, thickness, half, color);
		if (segments[digit][6]) Graphics::DrawRect(x + thickness, y + half - thickness * 0.5f, length, thickness, color);
	}

	void DrawStarCount()
	{
		const int collectedCount = (stageStarCollected ? 1 : 0) + (puzzleStarCollected ? 1 : 0);
		const float x = 24.0f;
		const float y = 24.0f;
		const float digitSize = 28.0f;
		const ColorF countColor(255, 215, 0);

		DrawDigit(collectedCount, x, y, digitSize, countColor);
		Graphics::DrawRect(x + 54.0f, y + 13.0f, 24.0f, 4.0f, Color::White);
		DrawDigit(2, x + 84.0f, y, digitSize, Color::White);
	}

	void OnStageCleared()
	{
		if (puzzleStarCollected) return;

		if (simulator.CheckPuzzleStarCondition(
			PuzzleConstants::PuzzleStarTargetX,
			PuzzleConstants::PuzzleStarTargetY))
		{
			puzzleStarCollected = true;
			std::cout << "\n★ スター2を獲得しました！\n";
		}
	}

	//=======
	//初期化
	//=======
	void Init()
	{
		Input::BindAction(Cmd::MoveLeft, { Key::Left, Key::A }, { Button::Left });
		Input::BindAction(Cmd::MoveRight, { Key::Right, Key::D }, { Button::Right });

		cout << "初期化完了\n";

		texPlayer = Graphics::LoadTexture(L"assets\\ui.png");
		texWhite = Graphics::LoadTexture(L"assets\\siroiro.png");

		// 起動時のシーンをタイトルにする
		SceneManager::ChangeScene(new TitleScene());
	}

	void Update()
	{

		//  パズル配置フェーズ
		// ==========================================
		if (currentPhase == GamePhase::Setup)
		{
			bool puzzleUpdated = false;

            // WASDまたは矢印キーによるパズル操作
			puzzleUpdated = simulator.UpdateInput();

			// スペースキーでブロックを掴む・置く
			bool isSpaceCurrent = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
			if (isSpaceCurrent && !isSpacePressedLastFrame) {
				simulator.ToggleGrab();
				puzzleUpdated = true;
			}
			isSpacePressedLastFrame = isSpaceCurrent;

			// リセットキー（Rキー）
			if ((GetAsyncKeyState('R') & 0x8000)) {
				simulator.ResetBlocks();
				puzzleUpdated = true;
			}

			// プレイ開始キー（Enterキーでアクション側に移行）
			if ((GetAsyncKeyState(VK_RETURN) & 0x8000)) {
				currentPhase = GamePhase::Playing;
				currentTick = 1;
				tickTimer = 0.0f;
				needsRedraw = true;
			}

			if (puzzleUpdated) {
				needsRedraw = true;
			}

			// 盤面データを判定システムに同期
			BlockCell currentGrid[GRID_WIDTH][GRID_HEIGHT];
			simulator.GetCurrentGridData(currentGrid);
			judgeSystem.SetCompletedGrid(currentGrid);
		}

		// ② アクション操作・ティック中 
		// ==========================================
		else if (currentPhase == GamePhase::Playing)
		{
			// メンバーが書いたプレイヤー移動処理
			if (Input::IsHeld(Cmd::MoveLeft)) {
				playerX -= 5.0f;
			}
			if (Input::IsHeld(Cmd::MoveRight)) {
				playerX += 5.0f;
			}

			// スピードを落としたタイマーで順番にループチェック
			tickTimer += 1.0f / 60.0f;
			if (tickTimer >= tickInterval) {
				tickTimer = 0.0f;
				currentTick++;
				if (currentTick > GRID_WIDTH) {
					currentTick = 1; // 1〜5列目を周回
				}
				needsRedraw = true;
			}

			// Qキーで配置フェーズに戻る
			if ((GetAsyncKeyState('Q') & 0x8000)) {
				currentPhase = GamePhase::Setup;
				needsRedraw = true;
			}

			if (!stageStarCollected &&
				IsOverlapping(
					playerX,
					playerY,
					100.0f,
					100.0f,
					PuzzleConstants::StageStarX,
					PuzzleConstants::StageStarY,
					PuzzleConstants::StageStarSize,
					PuzzleConstants::StageStarSize))
			{
				stageStarCollected = true;
				std::cout << "\n★ スター1を獲得しました！\n";
			}
		}

		SceneManager::Update();
	}

	void Draw()
	{
		SceneManager::Draw();
	}

}