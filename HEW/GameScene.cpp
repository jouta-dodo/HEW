#include "GameScene.h"
#include <iostream>

using namespace std;

bool GameScene::IsOverlapping(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2;
}

void GameScene::DrawDigit(int digit, float x, float y, float size, const ColorF& color)
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

void GameScene::DrawStarCount()
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

void GameScene::OnStageCleared()
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

void GameScene::Init()
{
    std::cout << "=== パズルゲーム本体初期化 ===\n";
    //========================================================================================================================================
    // ActionBindの書き方
    // Input::BindAction(Cmd::コマンド名,{Key::キーの名前(キーの名前はInput.cpp参照),{Button::ボタンの名前(キーと同じくInput.cpp参照))})}
    // 使い方これだけ簡単
    //========================================================================================================================================

    texPlayer = Graphics::LoadTexture(L"assets\\ui.png");
    texWhite = Graphics::LoadTexture(L"assets\\siroiro.png");
    player.Init(texPlayer);
    bo.Init(texWhite);
}

void GameScene::Update()
{
    //  パズル配置フェーズ
    // ==========================================

    bo.Update();
    /*
    
    */
    if (currentPhase == GamePhase::Setup)
    {
        // 配置フェーズ中もプレイヤーに重力だけを適用する
        player.UpdateGravity(PuzzleConstants::WindowWidth, 720.0f);
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
        bool isSpaceCurrent = Input::IsKeyHeld(Key::Space);
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

        // プレイ開始キー（Eキーでアクション側に移行）
        if (Input::IsKeyPressed(Key::E)) {
            currentPhase = GamePhase::Playing;
            currentTick = 1;
            tickTimer = 0.0f;
            needsRedraw = true;

            // プレイ開始時に一度だけコンソールを整理
            system("cls");
            cout << "=== 【フェーズ: アクション・プレイ中】 ===\n";
            cout << "([E] キーで配置フェーズに戻ります)\n\n";
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
        player.Update(PuzzleConstants::WindowWidth, 720.0f);

        // Rキーでプレイヤーのリスタート
        bool rNow = (GetAsyncKeyState('R') & 0x8000) != 0;
        if (rNow && !rWasPressed)
        {
            player.SetPos(590.0f, 310.0f);
            player.SetVelocityY(0.0f);
            player.SetGrounded(true);
            player.SetDead(false);
        }
        rWasPressed = rNow;

        // スピードを落としたタイマーで順番にループチェック
        tickTimer += 1.0f / 60.0f;
        if (tickTimer >= tickInterval) {
            tickTimer = 0.0f;
            currentTick++;
            if (currentTick > GRID_WIDTH) {
                currentTick = 1; // 1〜5列目を周回
            }

            // 【軽量化のポイント】
            // Playing中は画面クリア(system("cls"))を行わず、必要な判定評価のみ軽く出力するように変更
            cout << "現在のティック: " << currentTick << " 列目をチェック中\n";
            judgeSystem.EvaluateColumn(currentTick);
        }

        // Qキーで配置フェーズに戻る
        if ((GetAsyncKeyState('Q') & 0x8000)) {
            currentPhase = GamePhase::Setup;
            needsRedraw = true;
        }

        if (!stageStarCollected &&
            IsOverlapping(
                player.GetX(), player.GetY(), player.GetWidth(), player.GetHeight(),
                PuzzleConstants::StageStarX, PuzzleConstants::StageStarY,
                PuzzleConstants::StageStarSize, PuzzleConstants::StageStarSize))
        {
            stageStarCollected = true;
            std::cout << "\n スター1を獲得しました！\n";
        }
    }
}

void GameScene::Draw()
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

    // プレイヤー画像の描画
    player.Draw();
    bo.Draw();
    // スター1の描画
    if (!stageStarCollected)
    {
        Graphics::DrawImage(
            PuzzleConstants::StageStarX, PuzzleConstants::StageStarY,
            PuzzleConstants::StageStarSize, PuzzleConstants::StageStarSize,
            texWhite, Color::Yellow);
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
            starX + starOffset, starY + starOffset,
            PuzzleConstants::PuzzleStarSize, PuzzleConstants::PuzzleStarSize,
            baseTex, Color::Yellow);
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
    // 【軽量化のポイント】
    // Playingフェーズ中は system("cls") を行わず、Setupフェーズで操作・配置変更があったときのみ
    // 画面全体をクリアして描画するように分離しました。
    if (currentPhase == GamePhase::Setup && needsRedraw)
    {
        system("cls");

        cout << "=== 【フェーズ: パズル配置】 ===\n";
        simulator.DrawGrid();

        if (heldIdx != -1) {
            cout << "\n状態: 【ブロック移動中】 (スペースキーで配置)\n";
        }
        else {
            cout << "\n状態: 【ブロック選択中】 (W/A/S/Dで選択, スペースで掴む)\n";
        }
        cout << "操作: [W/A/S/D]移動  [Space]決定/解除  [R]リセット  [Enter]プレイ開始\n";

        needsRedraw = false;
    }
}