#include "Game.h"
#include "Input.h"
#include "Graphics.h"
#include "Scene.h"
#include <iostream>

// パズル・判定モジュールおよび設定のインクルード
// ==========================================
#include "BlockData.h"
#include "PlacementSimulator.h"
#include "CountJudgeMock.h"
#include "PuzzleConstants.h" //統合した定数ファイル

#include "CSVLoad.h"

using namespace std;

//=================================================================================
//使い方★
//namespace Gameの中で必ず書く
// 前のやつと同じようにInit(初期化),Update(更新),Draw(描画)の三種の神器を使います
//例 タイトルシーン
//class TitleScene:public IScene
//{
//void Init()overrride<-これ絶対書いてね
//	{
//		std::cout<<"おなかすいた";
//	{
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
	
	CSVLoad blockdata;

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
		//========================================================================================================================================
		// ActionBindの書き方
		// Input::BindAction(Cmd::コマンド名,{Key::キーの名前(キーの名前はInput.cpp参照),{Button::ボタンの名前(キーと同じくInput.cpp参照))})}
		// 使い方これだけ簡単
		//========================================================================================================================================

		//バインド処理
		Input::BindAction(Cmd::MoveLeft, { Key::Left,Key::A }, { Button::Left });
		Input::BindAction(Cmd::MoveRight, { Key::Right, Key::D }, { Button::Right });
		cout << "初期化完了\n";

		//===========================================
		//ここにシーンの設定のやつ書く
		//SceneManager::ChangeScene(new TitleScene);
		//===========================================

		//==============
		//画像貼り付け
		//==============
		texPlayer = Graphics::LoadTexture(L"assets\\ui.png");
		texWhite = Graphics::LoadTexture(L"assets\\siroiro.png");

		blockdata.LoadBlockCSV("assets/stage1.csv");

		if (texPlayer.Get() == nullptr)
		{
			MessageBox(nullptr, L"画像の読み込み失敗", L"警告", MB_OK);
		}
		if (texWhite.Get() == nullptr)
		{
			MessageBox(nullptr, L"siroiro.png の読み込み失敗", L"警告", MB_OK);
		}
		else
		{
			MessageBox(nullptr, L"画像読み込み成功", L"成功", MB_OK);
		}
	}

	//=========
	//更新処理
	//=========
	void Update()
	{

		//  パズル配置フェーズ
		// ==========================================
		if (currentPhase == GamePhase::Setup)
		{
			bool puzzleUpdated = false;

			// WASDまたは矢印キーでブロック/フォーカス単位の移動
			if (Input::IsKeyPressed(Key::W) || Input::IsKeyPressed(Key::Up)) {
				if (simulator.GetHeldIndex() != -1) simulator.MoveHeldBlock(0, -1);
				else simulator.MoveFocus(0, -1);
				puzzleUpdated = true;
			}
			if (Input::IsKeyPressed(Key::S) || Input::IsKeyPressed(Key::Down)) {
				if (simulator.GetHeldIndex() != -1) simulator.MoveHeldBlock(0, 1);
				else simulator.MoveFocus(0, 1);
				puzzleUpdated = true;
			}
			if (Input::IsKeyPressed(Key::A) || Input::IsKeyPressed(Key::Left)) {
				if (simulator.GetHeldIndex() != -1) simulator.MoveHeldBlock(-1, 0);
				else simulator.MoveFocus(-1, 0);
				puzzleUpdated = true;
			}
			if (Input::IsKeyPressed(Key::D) || Input::IsKeyPressed(Key::Right)) {
				if (simulator.GetHeldIndex() != -1) simulator.MoveHeldBlock(1, 0);
				else simulator.MoveFocus(1, 0);
				puzzleUpdated = true;
			}

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
	}

	void Draw()
	{
		//========================================================================================
		// 待望の点線描画機能追加
		// 使い方
		// 普通に箱描画
		// そして下の形式で入力する
		// Graphics::DrawDashedBorder(playerX, playerY, 100.0f, 100.0f, Color::Red, 10.0f, 3.0f);
		// そしたら描画できます
		// =======================================================================================
		
		// 左上にスターの獲得数を表示
		DrawStarCount();

		// メンバーが作ったプレイヤー画像の描画
		Graphics::DrawImage(playerX, playerY, 100.0f, 100.0f, texPlayer, Color::White);

		// スター1の描画
		if (!stageStarCollected)
		{
			Graphics::DrawImage(
				PuzzleConstants::StageStarX,
				PuzzleConstants::StageStarY,
				PuzzleConstants::StageStarSize,
				PuzzleConstants::StageStarSize,
				texWhite,
				Color::Yellow);
		}

		// =====================================================================
		// ★ゲーム画面の右端に綺麗に整えられたパズル盤面を描画
		// =====================================================================
		float cellSize = PuzzleConstants::CellSize;
		float cellSpacing = PuzzleConstants::CellSpacing;
		float rightMargin = PuzzleConstants::RightMargin;

		float totalPuzzleWidth = GRID_WIDTH * cellSize + (GRID_WIDTH - 1) * cellSpacing;
		float puzzleStartX = PuzzleConstants::WindowWidth - totalPuzzleWidth - rightMargin;
		float puzzleStartY = PuzzleConstants::PuzzleStartY;

		ComPtr<ID3D11ShaderResourceView> baseTex = (texWhite.Get() != nullptr) ? texWhite : texPlayer;

		ColorF woodColor(0.55f, 0.38f, 0.24f, 1.0f); // 木製風ブラウン
		ColorF wallColor(0.35f, 0.35f, 0.35f, 1.0f); // 壁用の濃い灰色

		// 盤面データを取得
		BlockCell currentGrid[GRID_WIDTH][GRID_HEIGHT];
		simulator.GetCurrentGridData(currentGrid);

		int focusedIdx = simulator.GetFocusedBlockIndex();
		int heldIdx = simulator.GetHeldIndex();
		const auto& blocks = simulator.GetBlocks();

		// 1. 各セルのベース（背景・壁・固定ギミック・ブロック）を描画
		for (int x = 0; x < GRID_WIDTH; x++)
		{
			for (int y = 0; y < GRID_HEIGHT; y++)
			{
				float drawX = puzzleStartX + x * (cellSize + cellSpacing);
				float drawY = puzzleStartY + y * (cellSize + cellSpacing);

				// ① 壁のチェック
				if (simulator.IsWall(x, y)) {
					Graphics::DrawImage(drawX, drawY, cellSize, cellSize, baseTex, wallColor);
				}
				// ② ブロックが存在する場合
				else if (currentGrid[x][y].hasBlock) {
					ColorF cellColor = GetColorFromFramework(currentGrid[x][y].color);
					Graphics::DrawImage(drawX, drawY, cellSize, cellSize, baseTex, cellColor);
				}
				// ③ 固定ギミックのチェック（ブロックが乗っていない場合）
				else {
					FrameworkColor gColor = simulator.GetFixedGimmick(x, y);
					if (gColor != FrameworkColor::None) {
						ColorF gimmickBorderColor = GetColorFromFramework(gColor);
						float fw = PuzzleConstants::FrameWidth;
						Graphics::DrawImage(drawX - fw, drawY - fw, cellSize + fw * 2.0f, cellSize + fw * 2.0f, baseTex, gimmickBorderColor);
						Graphics::DrawImage(drawX, drawY, cellSize, cellSize, baseTex, Color::White);
					}
					else {
						Graphics::DrawImage(drawX, drawY, cellSize, cellSize, baseTex, woodColor);
					}
				}
			}
		}

		// スター2の描画（壁の右横）
		if (!puzzleStarCollected)
		{
			float starX = puzzleStartX + PuzzleConstants::PuzzleStarTargetX * (cellSize + cellSpacing);
			float starY = puzzleStartY + PuzzleConstants::PuzzleStarTargetY * (cellSize + cellSpacing);
			float starOffset = (cellSize - PuzzleConstants::PuzzleStarSize) / 2.0f;
			Graphics::DrawImage(
				starX + starOffset,
				starY + starOffset,
				PuzzleConstants::PuzzleStarSize,
				PuzzleConstants::PuzzleStarSize,
				baseTex,
				Color::Yellow);
		}

		// 2. 選択中のブロックに対する「白枠（またはオレンジ枠）」のハイライト表示
		if (currentPhase == GamePhase::Setup)
		{
			int targetIdx = (heldIdx != -1) ? heldIdx : focusedIdx;
			if (targetIdx >= 0 && targetIdx < (int)blocks.size())
			{
				const auto& block = blocks[targetIdx];
				for (int ly = 0; ly < 3; ly++)
				{
					for (int lx = 0; lx < 3; lx++)
					{
						if (block.shape[ly][lx] == 1)
						{
							int gx = block.x + lx;
							int gy = block.y + ly;
							if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT)
							{
								float drawX = puzzleStartX + gx * (cellSize + cellSpacing);
								float drawY = puzzleStartY + gy * (cellSize + cellSpacing);

								float frameWidth = PuzzleConstants::FrameWidth;
								ColorF frameColor = (heldIdx != -1) ? Color::Orange : Color::White;

								Graphics::DrawImage(drawX - frameWidth, drawY - frameWidth, cellSize + frameWidth * 2.0f, cellSize + frameWidth * 2.0f, baseTex, frameColor);

								ColorF cellColor = GetColorFromFramework(currentGrid[gx][gy].color);
								Graphics::DrawImage(drawX, drawY, cellSize, cellSize, baseTex, cellColor);
							}
						}
					}
				}
			}
		}

		// プレイ中のハイライト（現在チェック中の列）
		if (currentPhase == GamePhase::Playing)
		{
			for (int y = 0; y < GRID_HEIGHT; y++)
			{
				int gx = currentTick - 1;
				if (gx >= 0 && gx < GRID_WIDTH)
				{
					float drawX = puzzleStartX + gx * (cellSize + cellSpacing);
					float drawY = puzzleStartY + y * (cellSize + cellSpacing);
					float tickWidth = PuzzleConstants::FrameWidth;
					Graphics::DrawImage(drawX - tickWidth, drawY - tickWidth, cellSize + tickWidth * 2.0f, cellSize + tickWidth * 2.0f, baseTex, Color::Cyan);

					if (currentGrid[gx][y].hasBlock) {
						ColorF cellColor = GetColorFromFramework(currentGrid[gx][y].color);
						Graphics::DrawImage(drawX, drawY, cellSize, cellSize, baseTex, cellColor);
					}
				}
			}
		}

		// 3. パズル盤面のすぐ下に「その列にある複数のブロックの色」を積み上げて描画
		float resultBarY = puzzleStartY + GRID_HEIGHT * (cellSize + cellSpacing) + 12.0f;
		float subBoxSize = cellSize / 3.0f - 2.0f;

		for (int x = 0; x < GRID_WIDTH; x++)
		{
			float drawX = puzzleStartX + x * (cellSize + cellSpacing);

			if (currentPhase == GamePhase::Playing && (x + 1 == currentTick)) {
				Graphics::DrawImage(drawX - 3.0f, resultBarY - 3.0f, cellSize + 6.0f, (subBoxSize + 2.0f) * 3.0f + 6.0f, baseTex, Color::Cyan);
			}

			int blockCount = 0;
			for (int y = 0; y < GRID_HEIGHT; y++) {
				if (currentGrid[x][y].hasBlock) {
					ColorF blockColor = GetColorFromFramework(currentGrid[x][y].color);
					float subY = resultBarY + blockCount * (subBoxSize + 2.0f);
					Graphics::DrawImage(drawX, subY, cellSize, subBoxSize, baseTex, blockColor);
					blockCount++;
				}
			}
			if (blockCount == 0) {
				Graphics::DrawImage(drawX, resultBarY, cellSize, 18.0f, baseTex, Color::Gray);
			}
		}


		// ==========================================
		// コンソール画面側の描画（確認用ログ）
		// ==========================================
		if (needsRedraw)
		{
			system("cls");

			if (currentPhase == GamePhase::Setup)
			{
				cout << "=== 【フェーズ: パズル配置】 ===\n";
				simulator.DrawGrid();

				if (heldIdx != -1) {
					cout << "\n状態: 【ブロック移動中】 (スペースキーで配置)\n";
				}
				else {
					cout << "\n状態: 【ブロック選択中】 (W/A/S/Dで選択, スペースで掴む)\n";
				}
				cout << "操作: [W/A/S/D]移動  [Space]決定/解除  [R]リセット  [Enter]プレイ開始\n";
			}
			else if (currentPhase == GamePhase::Playing)
			{
				cout << "ティック 1-5 周回中（ゲーム画面右端で操作・確認できます）\n";
				cout << "([Q] キーを押すと配置フェーズに戻ります)\n\n";

				cout << "現在のティック: " << currentTick << " 列目をチェック中\n";
				judgeSystem.EvaluateColumn(currentTick);
			}

			needsRedraw = false;
		}
	}

}
//平岡作player操作　合わせ
/*
#include "Game.h"
void Game::Init(HWND hWnd)
{
    RendererInit(hWnd);
    // プレイヤー
    m_player.Init();
    m_player.SetPos(-270.0f, -155.0f, 0.0f);
    m_player.SetSize(30.0f, 40.0f, 1.0f);
    m_player.SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    // 地面
    m_ground.Init();
    m_ground.SetPos(0.0f, -220.0f, 0.0f);
    m_ground.SetSize(640.0f, 40.0f, 1.0f);
    m_ground.SetColor(0.28f, 0.28f, 0.28f, 1.0f);
    // 左上の固定足場
    m_fixedPlatform.Init();
    m_fixedPlatform.SetPos(-234.0f, 7.0f, 0.0f);
    m_fixedPlatform.SetSize(176.0f, 70.0f, 1.0f);
    m_fixedPlatform.SetColor(0.28f, 0.28f, 0.28f, 1.0f);
    // 赤①
    m_redLeftPlatform.Init();
    m_redLeftPlatform.SetPos(-146.0f, -168.0f, 0.0f);
    m_redLeftPlatform.SetSize(126.0f, 64.0f, 1.0f);
    m_redLeftPlatform.SetColor(1.0f, 0.35f, 0.35f, 1.0f);
    // 緑
    m_greenPlatform.Init();
    m_greenPlatform.SetPos(0.0f, -143.0f, 0.0f);
    m_greenPlatform.SetSize(126.0f, 114.0f, 1.0f);
    m_greenPlatform.SetColor(0.0f, 1.0f, 0.0f, 1.0f);
    // 黄色
    m_yellowPlatform.Init();
    m_yellowPlatform.SetPos(110.0f, -113.0f, 0.0f);
    m_yellowPlatform.SetSize(204.0f, 174.0f, 1.0f);
    m_yellowPlatform.SetColor(1.0f, 0.85f, 0.0f, 1.0f);
    // 赤②
    m_redRightPlatform.Init();
    m_redRightPlatform.SetPos(173.0f, -143.0f, 0.0f);
    m_redRightPlatform.SetSize(222.0f, 114.0f, 1.0f);
    m_redRightPlatform.SetColor(1.0f, 0.35f, 0.35f, 1.0f);
    // 紫
    m_purplePlatform.Init();
    m_purplePlatform.SetPos(-50.0f, 7.0f, 0.0f);
    m_purplePlatform.SetSize(196.0f, 70.0f, 1.0f);
    m_purplePlatform.SetColor(0.65f, 0.2f, 0.9f, 1.0f);
    // ガイドの色
    // 薄いグレーで出現位置を表示する
    // 赤①ガイド
    m_redLeftGuide.Init();
    m_redLeftGuide.SetPos(-146.0f, -168.0f, 0.0f);
    m_redLeftGuide.SetSize(126.0f, 64.0f, 1.0f);
    m_redLeftGuide.SetColor(0.65f, 0.65f, 0.65f, 1.0f);
    // 緑ガイド
    m_greenGuide.Init();
    m_greenGuide.SetPos(0.0f, -143.0f, 0.0f);
    m_greenGuide.SetSize(126.0f, 114.0f, 1.0f);
    m_greenGuide.SetColor(0.65f, 0.65f, 0.65f, 1.0f);
    // 黄色ガイド
    m_yellowGuide.Init();
    m_yellowGuide.SetPos(110.0f, -113.0f, 0.0f);
    m_yellowGuide.SetSize(204.0f, 174.0f, 1.0f);
    m_yellowGuide.SetColor(0.65f, 0.65f, 0.65f, 1.0f);
    // 赤②ガイド
    m_redRightGuide.Init();
    m_redRightGuide.SetPos(173.0f, -143.0f, 0.0f);
    m_redRightGuide.SetSize(222.0f, 114.0f, 1.0f);
    m_redRightGuide.SetColor(0.65f, 0.65f, 0.65f, 1.0f);
    // 紫ガイド
    m_purpleGuide.Init();
    m_purpleGuide.SetPos(-50.0f, 7.0f, 0.0f);
    m_purpleGuide.SetSize(196.0f, 70.0f, 1.0f);
    m_purpleGuide.SetColor(0.65f, 0.65f, 0.65f, 1.0f);

    // ガイドの縁を作る
    auto SetGuideFrame = [&](int index,
        float x, float y, float w, float h,
        float r, float g, float b)
        {
            float t = 3.0f; // 縁の太さ
            // 上
            m_guideFrames[index][0].Init();
            m_guideFrames[index][0].SetPos(x, y + h / 2.0f - t / 2.0f, 0.0f);
            m_guideFrames[index][0].SetSize(w, t, 1.0f);
            m_guideFrames[index][0].SetColor(r, g, b, 1.0f);
            // 下
            m_guideFrames[index][1].Init();
            m_guideFrames[index][1].SetPos(x, y - h / 2.0f + t / 2.0f, 0.0f);
            m_guideFrames[index][1].SetSize(w, t, 1.0f);
            m_guideFrames[index][1].SetColor(r, g, b, 1.0f);
            // 左
            m_guideFrames[index][2].Init();
            m_guideFrames[index][2].SetPos(x - w / 2.0f + t / 2.0f, y, 0.0f);
            m_guideFrames[index][2].SetSize(t, h, 1.0f);
            m_guideFrames[index][2].SetColor(r, g, b, 1.0f);
            // 右
            m_guideFrames[index][3].Init();
            m_guideFrames[index][3].SetPos(x + w / 2.0f - t / 2.0f, y, 0.0f);
            m_guideFrames[index][3].SetSize(t, h, 1.0f);
            m_guideFrames[index][3].SetColor(r, g, b, 1.0f);
        };
    // 赤①
    SetGuideFrame(0, -146.0f, -168.0f, 126.0f, 64.0f, 1.0f, 0.35f, 0.35f);
    // 緑
    SetGuideFrame(1, 0.0f, -143.0f, 126.0f, 114.0f, 0.0f, 0.85f, 0.9f);
    // 黄
    SetGuideFrame(2, 110.0f, -113.0f, 204.0f, 174.0f, 1.0f, 0.85f, 0.0f);
    // 赤②
    SetGuideFrame(3, 173.0f, -143.0f, 222.0f, 114.0f, 1.0f, 0.35f, 0.35f);
    // 紫
    SetGuideFrame(4, -50.0f, 7.0f, 196.0f, 70.0f, 0.65f, 0.2f, 0.9f);}
void Game::Update(void)
{
    input.Update();
    // 死亡中の処理

    // Rキーでリスタート
    bool rNow = input.GetKeyPress(VK_R);
    if (rNow && !m_rWasPressed)
    {
        // プレイヤーを初期位置に戻す
        m_player.SetPos(-270.0f, -180.0f, 0.0f);
        // プレイヤーの色を黒に戻す
        m_player.SetColor(0.0f, 0.0f, 0.0f, 1.0f);
        // ジャンプ状態をリセット
        m_velocityY = 0.0f;
        m_isGrounded = true;
        m_spaceWasPressed = false;
        // ブロックの番号とタイマーをリセット
        m_blockNumber = 1;
        m_blockTimer = 0;
        // 死亡状態を解除
        m_isDead = false;
        m_rWasPressed = rNow;
        return;
    }
    m_rWasPressed = rNow;

    if (m_isDead)
    {
        /*if (input.GetKeyPress(VK_R))
        {
            m_isDead = false;
            m_player.SetPos(-270.0f, -155.0f, 0.0f);
            m_player.SetColor(0.0f, 0.0f, 0.0f, 1.0f);
            m_velocityY = 0.0f;
            m_isGrounded = true;
            m_spaceWasPressed = false;
        }*/


/*
return;
    }
    // ブロック切り替え
    m_blockTimer++;
    if (m_blockTimer >= 90)
    {
        m_blockTimer = 0;
        m_blockNumber++;
        if (m_blockNumber > 4)
        {
            m_blockNumber = 1;
        }
    }


    // 消える前の20フレームでフェード
    float fade = 0.0f;
    if (m_blockTimer >= 70)
    {
        fade = (m_blockTimer - 70) / 20.0f;
    }
    // 元の色からガイドのグレー色へ徐々に変える
    auto FadePlatform = [&](Object& platform,
        float r, float g, float b)
        {
            float gray = 0.65f;
            float newR = r + (gray - r) * fade;
            float newG = g + (gray - g) * fade;
            float newB = b + (gray - b) * fade;
            platform.SetColor(newR, newG, newB, 1.0f);
        };
    // 現在のブロックをフェード
    if (m_blockNumber == 1)
    {
        FadePlatform(m_redLeftPlatform, 1.0f, 0.35f, 0.35f);
        FadePlatform(m_redRightPlatform, 1.0f, 0.35f, 0.35f);
    }
    else if (m_blockNumber == 2)
    {
        FadePlatform(m_greenPlatform, 0.0f, 1.0f, 0.0f);
    }
    else if (m_blockNumber == 3)
    {
        FadePlatform(m_yellowPlatform, 1.0f, 0.85f, 0.0f);
    }
    else if (m_blockNumber == 4)
    {
        FadePlatform(m_purplePlatform, 0.65f, 0.2f, 0.9f);
    }


    // 現在出現しているブロック
    Object* activePlatforms[2];
    int activeCount = 0;
    if (m_blockNumber == 1)
    {
        activePlatforms[activeCount++] = &m_redLeftPlatform;
        activePlatforms[activeCount++] = &m_redRightPlatform;
    }
    else if (m_blockNumber == 2)
    {
        activePlatforms[activeCount++] = &m_greenPlatform;
    }
    else if (m_blockNumber == 3)
    {
        activePlatforms[activeCount++] = &m_yellowPlatform;
    }
    else if (m_blockNumber == 4)
    {
        activePlatforms[activeCount++] = &m_purplePlatform;
    }
    // プレイヤー情報
    DirectX::XMFLOAT3 pos = m_player.GetPos();
    DirectX::XMFLOAT3 size = m_player.GetSize();
    float halfWidth = size.x / 2.0f;
    float halfHeight = size.y / 2.0f;
    // 地面情報
    DirectX::XMFLOAT3 groundPos = m_ground.GetPos();
    DirectX::XMFLOAT3 groundSize = m_ground.GetSize();
    DirectX::XMFLOAT3 previousPos = pos;
    float groundTop = groundPos.y + groundSize.y / 2.0f;
    // 足場の上にいるか確認
    auto IsStandingOn = [&](Object& platform)
        {
            auto p = platform.GetPos();
            auto s = platform.GetSize();
            float left = p.x - s.x / 2.0f;
            float right = p.x + s.x / 2.0f;
            float top = p.y + s.y / 2.0f;
            float bottom = pos.y - halfHeight;
            return pos.x + halfWidth > left &&
                pos.x - halfWidth < right &&
                bottom >= top - 1.0f &&
                bottom <= top + 1.0f;
        };
    // 着地状態
    m_isGrounded =
        (pos.y - halfHeight >= groundTop - 1.0f &&
            pos.y - halfHeight <= groundTop + 1.0f);
    if (IsStandingOn(m_fixedPlatform))
    {
        m_isGrounded = true;
    }
    if (m_blockTimer < 90)
    {
        for (int i = 0; i < activeCount; i++)
        {
            if (IsStandingOn(*activePlatforms[i]))
            {
                m_isGrounded = true;
            }
        }
    }
    // ジャンプ
    bool spaceNow = input.GetKeyPress(VK_SPACE);
    if (spaceNow && !m_spaceWasPressed && m_isGrounded)
    {
        m_velocityY = m_jumpPower;
        m_isGrounded = false;
    }
    m_spaceWasPressed = spaceNow;
    // 左右移動w
    if (input.GetKeyPress(VK_A) ||
        input.GetKeyPress(VK_LEFT))
    {
        pos.x -= m_moveSpeed;
    }
    if (input.GetKeyPress(VK_D) ||
        input.GetKeyPress(VK_RIGHT))
    {
        pos.x += m_moveSpeed;
    }
    // 画面外制限
    if (pos.x < -SCREEN_WIDTH / 2.0f + halfWidth)
        pos.x = -SCREEN_WIDTH / 2.0f + halfWidth;
    if (pos.x > SCREEN_WIDTH / 2.0f - halfWidth)
        pos.x = SCREEN_WIDTH / 2.0f - halfWidth;
    // 重力
    if (!m_isGrounded)
    {
        m_velocityY -= m_gravity;
        pos.y += m_velocityY;
    }
    else
    {
        m_velocityY = 0.0f;
    }
    // 足場との当たり判定
    auto CheckCollision = [&](Object& platform)
        {
            auto p = platform.GetPos();
            auto s = platform.GetSize();
            float left = p.x - s.x / 2.0f;
            float right = p.x + s.x / 2.0f;
            float top = p.y + s.y / 2.0f;
            float bottom = p.y - s.y / 2.0f;
            float playerLeft = pos.x - halfWidth;
            float playerRight = pos.x + halfWidth;
            float playerTop = pos.y + halfHeight;
            float playerBottom = pos.y - halfHeight;
            float previousBottom = previousPos.y - halfHeight;
            float previousTop = previousPos.y + halfHeight;
            float previousLeft = previousPos.x - halfWidth;
            float previousRight = previousPos.x + halfWidth;
            // 横方向の重なり
            float overlapX =
                (playerRight < right ? playerRight : right) -
                (playerLeft > left ? playerLeft : left);
            // 縦方向の重なり
            float overlapY =
                (playerTop < top ? playerTop : top) -
                (playerBottom > bottom ? playerBottom : bottom);
            if (overlapX <= 0.0f || overlapY <= 0.0f)
                return;
            // めり込みすぎたら死亡
            if (overlapX >= halfWidth &&
                overlapY >= halfHeight)
            {
                m_isDead = true;
                m_player.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
                return;
            }
            // 上から落ちてきた場合だけ着地
            if (m_velocityY <= 0.0f &&
                previousBottom >= top &&
                playerBottom <= top)
            {
                pos.y = top + halfHeight;
                m_velocityY = 0.0f;
                m_isGrounded = true;
                return;
            }
            // 下からブロックにぶつかった場合
            if (m_velocityY > 0.0f &&
                previousTop <= bottom &&
                playerTop >= bottom)
            {
                pos.y = bottom - halfHeight;
                m_velocityY = 0.0f;
                return;
            }
            // 横からぶつかった場合
            if (previousRight <= left)
            {
                pos.x = left - halfWidth;
            }
            else if (previousLeft >= right)
            {
                pos.x = right + halfWidth;
            }
            else
            {
                // 横からの衝突を優先して押し出す
                if (pos.x < p.x)
                    pos.x = left - halfWidth;
                else
                    pos.x = right + halfWidth;
            }
        };
    // 固定足場との当たり判定
    CheckCollision(m_fixedPlatform);
    // 出現中のブロックとの当たり判定
    if (m_blockTimer < 90)
    {
        for (int i = 0; i < activeCount; i++)
        {
            CheckCollision(*activePlatforms[i]);
            if (m_isDead)
            {
                m_player.SetPos(pos.x, pos.y, pos.z);
                return;
            }
        }
    }
    if (m_isDead)
    {
        m_player.SetPos(pos.x, pos.y, pos.z);
        return;
    }
    // 地面との当たり判定
    float playerBottom = pos.y - halfHeight;
    if (playerBottom <= groundTop)
    {
        pos.y = groundTop + halfHeight;
        m_velocityY = 0.0f;
        m_isGrounded = true;
    }
    // プレイヤー位置更新
    m_player.SetPos(pos.x, pos.y, pos.z);
}
void Game::Draw(void)
{
    RendererDrawStart();
    // 地面
    m_ground.Draw();
    // 固定足場
    m_fixedPlatform.Draw();
    // 出現位置のガイド
    m_redLeftGuide.Draw();
    m_greenGuide.Draw();
    m_yellowGuide.Draw();
    m_redRightGuide.Draw();
    m_purpleGuide.Draw();
    // 数字に対応したブロック
    if (m_blockNumber == 1)
    {
        m_redLeftPlatform.Draw();
        m_redRightPlatform.Draw();
    }
    else if (m_blockNumber == 2)
    {
        m_greenPlatform.Draw();
    }
    else if (m_blockNumber == 3)
    {
        m_yellowPlatform.Draw();
    }
    else if (m_blockNumber == 4)
    {
        m_purplePlatform.Draw();
    }

    // ガイドの縁
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m_guideFrames[i][j].Draw();
        }
    }
    // プレイヤー
    m_player.Draw();
    RendererDrawEnd();
}
void Game::Uninit(void)
{
    m_player.Uninit();
    m_ground.Uninit();
    m_fixedPlatform.Uninit();
    m_redLeftPlatform.Uninit();
    m_greenPlatform.Uninit();
    m_yellowPlatform.Uninit();
    m_redRightPlatform.Uninit();
    m_purplePlatform.Uninit();
    m_redLeftGuide.Uninit();
    m_greenGuide.Uninit();
    m_yellowGuide.Uninit();
    m_redRightGuide.Uninit();
    m_purpleGuide.Uninit();
    RendererUninit();

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m_guideFrames[i][j].Uninit();
        }
    }
}
*/