#include "GameScene.h"
#include "StageSelectScene.h"

#include <iostream>

using namespace std;

bool GameScene::IsOverlapping(
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

void GameScene::DrawDigit(
    int digit,
    float x,
    float y,
    float size,
    const ColorF& color)
{
    // スターの数を表示するための7セグメント表示。
    // 線の番号:
    // 0=上、1=右上、2=右下、3=下、
    // 4=左下、5=左上、6=中央
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

    if (digit < 0 || digit > 9)
    {
        return;
    }

    float thickness = size * 0.18f;
    float length = size;
    float half = size * 0.5f;

    if (segments[digit][0])
    {
        Graphics::DrawRect(
            x + thickness,
            y,
            length,
            thickness,
            color);
    }

    if (segments[digit][1])
    {
        Graphics::DrawRect(
            x + length,
            y + thickness,
            thickness,
            half,
            color);
    }

    if (segments[digit][2])
    {
        Graphics::DrawRect(
            x + length,
            y + half,
            thickness,
            half,
            color);
    }

    if (segments[digit][3])
    {
        Graphics::DrawRect(
            x + thickness,
            y + size,
            length,
            thickness,
            color);
    }

    if (segments[digit][4])
    {
        Graphics::DrawRect(
            x,
            y + half,
            thickness,
            half,
            color);
    }

    if (segments[digit][5])
    {
        Graphics::DrawRect(
            x,
            y + thickness,
            thickness,
            half,
            color);
    }

    if (segments[digit][6])
    {
        Graphics::DrawRect(
            x + thickness,
            y + half - thickness * 0.5f,
            length,
            thickness,
            color);
    }
}

void GameScene::DrawStarCount()
{
    const int collectedCount =
        (stageStarCollected ? 1 : 0) +
        (clearStarCollected ? 1 : 0) +
        (tickStarCollected ? 1 : 0);

    const float x = 24.0f;
    const float y = 24.0f;
    const float digitSize = 28.0f;
    const ColorF countColor(255, 215, 0);

    DrawDigit(
        collectedCount,
        x,
        y,
        digitSize,
        countColor);

    Graphics::DrawRect(
        x + 54.0f,
        y + 13.0f,
        24.0f,
        4.0f,
        Color::White);

    DrawDigit(
        3,
        x + 84.0f,
        y,
        digitSize,
        Color::White);
}

void GameScene::Init()
{
    std::cout << "=== パズルゲーム本体初期化 ===\n";

    texPlayer = Graphics::LoadTexture(L"assets\\ui.png");
    texWhite = Graphics::LoadTexture(L"assets\\siroiro.png");

    player.Init(texPlayer);
    bo.Init(texWhite);
}

void GameScene::Update()
{
    bo.Update();

    // パズル配置フェーズ
    if (currentPhase == GamePhase::Setup)
    {
        player.UpdateGravity(
            PuzzleConstants::WindowWidth,
            720.0f);

        bool puzzleUpdated = false;

        puzzleUpdated = simulator.UpdateInput();

        bool isSpaceCurrent = Input::IsKeyHeld(Key::Space);

        if (isSpaceCurrent && !isSpacePressedLastFrame)
        {
            simulator.ToggleGrab();
            puzzleUpdated = true;
        }

        if (Input::IsKeyPressed(Key::C))
        {
            // Cキー：現在移動中のブロックだけを右下へ戻す。
            puzzleUpdated =
                simulator.ReturnHeldBlockToSource() || puzzleUpdated;
        }

        for (int sourceIndex = 0; sourceIndex < 6; sourceIndex++)
        {
            bo.SetSourceBlockVisible(
                sourceIndex,
                simulator.IsSourceBlockAvailable(sourceIndex));
        }

        isSpacePressedLastFrame = isSpaceCurrent;

        // パズルのリセット
        if ((GetAsyncKeyState('R') & 0x8000))
        {
            simulator.ResetBlocks();

            for (int sourceIndex = 0; sourceIndex < 6; sourceIndex++)
            {
                bo.SetSourceBlockVisible(sourceIndex, true);
            }

            puzzleUpdated = true;
        }

        // プレイ開始
        if (Input::IsKeyPressed(Key::E))
        {
            currentPhase = GamePhase::Playing;
            tickManager.Reset();
            needsRedraw = true;

            system("cls");

            cout << "=== 【フェーズ: アクション・プレイ中】 ===\n";
            cout << "([E] キーで配置フェーズに戻ります)\n\n";
        }

        if (puzzleUpdated)
        {
            needsRedraw = true;
        }

        // 盤面データを判定システムへ同期
        BlockCell currentGrid[GRID_WIDTH][GRID_HEIGHT];

        simulator.GetCurrentGridData(currentGrid);
        judgeSystem.SetCompletedGrid(currentGrid);
    }
    // アクション操作・ティック中
    else if (currentPhase == GamePhase::Playing)
    {
        if (resultShown)
        {
            if (Input::IsKeyPressed(Key::Space))
            {
                SceneManager::ChangeScene(new StageSelectScene());
            }

            return;
        }

        player.Update(
            PuzzleConstants::WindowWidth,
            720.0f);

        // プレイヤーのリスタート
        bool rNow = (GetAsyncKeyState('R') & 0x8000) != 0;

        if (rNow && !rWasPressed)
        {
            player.SetPos(590.0f, 310.0f);
            player.SetVelocityY(0.0f);
            player.SetGrounded(true);
            player.SetDead(false);
        }

         rWasPressed = rNow;

        // ティック更新と現在列の判定
        tickManager.Update(
            1.0f / 60.0f,
            judgeSystem);

        // Qキーで配置フェーズへ戻る
        if ((GetAsyncKeyState('Q') & 0x8000))
        {
            currentPhase = GamePhase::Setup;
            needsRedraw = true;
        }

        // スター1の取得判定
        if (!stageStarCollected &&
            IsOverlapping(
                player.GetX(),
                player.GetY(),
                player.GetWidth(),
                player.GetHeight(),
                PuzzleConstants::StageStarX,
                PuzzleConstants::StageStarY,
                PuzzleConstants::StageStarSize,
                PuzzleConstants::StageStarSize))
        {
            stageStarCollected = true;
            std::cout << "\nスター1を獲得しました！\n";
        }

        // ゴール判定
        if (!resultShown &&
            IsOverlapping(
                player.GetX(),
                player.GetY(),
                player.GetWidth(),
                player.GetHeight(),
                PuzzleConstants::GoalX,
                PuzzleConstants::GoalY,
                PuzzleConstants::GoalWidth,
                PuzzleConstants::GoalHeight))
        {
            clearStarCollected = true;

            // 5ティックループ以内のゴールスター
            if (tickManager.GetTickLoopCount() <= 5)
            {
                tickStarCollected = true;
            }

            resultShown = true;

            const int collectedCount =
                (stageStarCollected ? 1 : 0) +
                (clearStarCollected ? 1 : 0) +
                (tickStarCollected ? 1 : 0);

            std::cout << "\n ステージクリア \n";
            std::cout << "経過ティックループ: "
                      << tickManager.GetTickLoopCount()
                      << "\n";
            std::cout << "獲得スター: "
                      << collectedCount
                      << " / 3\n";
        }
    }
}

void GameScene::Draw()
{
    DrawStarCount();

    // ゴールオブジェクト
    Graphics::DrawImage(
        PuzzleConstants::GoalX,
        PuzzleConstants::GoalY,
        PuzzleConstants::GoalWidth,
        PuzzleConstants::GoalHeight,
        texWhite,
        Color::Green);

    player.Draw();
    bo.Draw();

    // CSVブロックの選択中枠
    if (simulator.IsSourceFocused())
    {
        const int sourceFocusIndex = simulator.GetSourceFocusIndex();
        const float sourceStartX = 800.0f;
        const float sourceStartY = 450.0f;
        const float sourceCellSize = 22.0f;
        const float sourceCellSpacing = 5.0f;
        const float sourceBlockSize =
            sourceCellSize * 3.0f + sourceCellSpacing * 2.0f;
        const float sourceX =
            sourceStartX + (sourceFocusIndex % 3) * 4.0f *
            (sourceCellSize + sourceCellSpacing);
        const float sourceY =
            sourceStartY + (sourceFocusIndex / 3) * 4.0f *
            (sourceCellSize + sourceCellSpacing);
        const float sourceFrameWidth = 3.0f;

        Graphics::DrawRect(
            sourceX - sourceFrameWidth,
            sourceY - sourceFrameWidth,
            sourceBlockSize + sourceFrameWidth * 2.0f,
            sourceFrameWidth,
            Color::White);
        Graphics::DrawRect(
            sourceX - sourceFrameWidth,
            sourceY + sourceBlockSize,
            sourceBlockSize + sourceFrameWidth * 2.0f,
            sourceFrameWidth,
            Color::White);
        Graphics::DrawRect(
            sourceX - sourceFrameWidth,
            sourceY,
            sourceFrameWidth,
            sourceBlockSize,
            Color::White);
        Graphics::DrawRect(
            sourceX + sourceBlockSize,
            sourceY,
            sourceFrameWidth,
            sourceBlockSize,
            Color::White);
    }

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

    float cellSize = PuzzleConstants::CellSize;
    float cellSpacing = PuzzleConstants::CellSpacing;
    float rightMargin = PuzzleConstants::RightMargin;

    float totalPuzzleWidth =
        GRID_WIDTH * cellSize +
        (GRID_WIDTH - 1) * cellSpacing;

    float puzzleStartX =
        PuzzleConstants::WindowWidth -
        totalPuzzleWidth -
        rightMargin;

    float puzzleStartY = PuzzleConstants::PuzzleStartY;

    ComPtr<ID3D11ShaderResourceView> baseTex =
        (texWhite.Get() != nullptr)
            ? texWhite
            : texPlayer;

    ColorF woodColor(
        0.55f,
        0.38f,
        0.24f,
        1.0f);

    ColorF wallColor(
        0.35f,
        0.35f,
        0.35f,
        1.0f);

    // 盤面データを取得
    BlockCell currentGrid[GRID_WIDTH][GRID_HEIGHT];
    simulator.GetCurrentGridData(currentGrid);

    int focusedIdx = simulator.GetFocusedBlockIndex();
    int heldIdx = simulator.GetHeldIndex();
    const auto& blocks = simulator.GetBlocks();

    // 各セルを描画
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            float drawX =
                puzzleStartX +
                x * (cellSize + cellSpacing);

            float drawY =
                puzzleStartY +
                y * (cellSize + cellSpacing);

            if (simulator.IsWall(x, y))
            {
                Graphics::DrawImage(
                    drawX,
                    drawY,
                    cellSize,
                    cellSize,
                    baseTex,
                    wallColor);
            }
            else if (currentGrid[x][y].hasBlock)
            {
                ColorF cellColor =
                    GetColorFromFramework(currentGrid[x][y].color);

                Graphics::DrawImage(
                    drawX,
                    drawY,
                    cellSize,
                    cellSize,
                    baseTex,
                    cellColor);
            }
            else
            {
                FrameworkColor gimmickColor =
                    simulator.GetFixedGimmick(x, y);

                if (gimmickColor != FrameworkColor::None)
                {
                    ColorF gimmickBorderColor =
                        GetColorFromFramework(gimmickColor);

                    float frameWidth =
                        PuzzleConstants::FrameWidth;

                    Graphics::DrawImage(
                        drawX - frameWidth,
                        drawY - frameWidth,
                        cellSize + frameWidth * 2.0f,
                        cellSize + frameWidth * 2.0f,
                        baseTex,
                        gimmickBorderColor);

                    Graphics::DrawImage(
                        drawX,
                        drawY,
                        cellSize,
                        cellSize,
                        baseTex,
                        Color::White);
                }
                else
                {
                    Graphics::DrawImage(
                        drawX,
                        drawY,
                        cellSize,
                        cellSize,
                        baseTex,
                        woodColor);
                }
            }
        }
    }

    // 配置フェーズ中の選択枠
    if (currentPhase == GamePhase::Setup)
    {
        int targetIdx =
            (heldIdx != -1)
                ? heldIdx
                : (simulator.IsSourceFocused() ? -1 : focusedIdx);

        if (targetIdx >= 0 &&
            targetIdx < static_cast<int>(blocks.size()))
        {
            const auto& block = blocks[targetIdx];

            for (int localY = 0; localY < 3; localY++)
            {
                for (int localX = 0; localX < 3; localX++)
                {
                    if (block.shape[localY][localX] == 1)
                    {
                        int gridX = block.x + localX;
                        int gridY = block.y + localY;

                        if (gridX >= 0 &&
                            gridX < GRID_WIDTH &&
                            gridY >= 0 &&
                            gridY < GRID_HEIGHT)
                        {
                            float drawX =
                                puzzleStartX +
                                gridX * (cellSize + cellSpacing);

                            float drawY =
                                puzzleStartY +
                                gridY * (cellSize + cellSpacing);

                            float frameWidth =
                                PuzzleConstants::FrameWidth;

                            ColorF frameColor =
                                (heldIdx != -1)
                                    ? Color::Orange
                                    : Color::White;

                            Graphics::DrawImage(
                                drawX - frameWidth,
                                drawY - frameWidth,
                                cellSize + frameWidth * 2.0f,
                                cellSize + frameWidth * 2.0f,
                                baseTex,
                                frameColor);

                            ColorF cellColor =
                                GetColorFromFramework(
                                    currentGrid[gridX][gridY].color);

                            Graphics::DrawImage(
                                drawX,
                                drawY,
                                cellSize,
                                cellSize,
                                baseTex,
                                cellColor);
                        }
                    }
                }

            }
        }
    }

    // プレイ中のティック対象列をハイライト
    if (currentPhase == GamePhase::Playing)
    {
        const int currentTick =
            tickManager.GetCurrentTick();

        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            int gridX = currentTick - 1;

            if (gridX >= 0 && gridX < GRID_WIDTH)
            {
                float drawX =
                    puzzleStartX +
                    gridX * (cellSize + cellSpacing);

                float drawY =
                    puzzleStartY +
                    y * (cellSize + cellSpacing);

                float tickWidth =
                    PuzzleConstants::FrameWidth;

                Graphics::DrawImage(
                    drawX - tickWidth,
                    drawY - tickWidth,
                    cellSize + tickWidth * 2.0f,
                    cellSize + tickWidth * 2.0f,
                    baseTex,
                    Color::Cyan);

                if (currentGrid[gridX][y].hasBlock)
                {
                    ColorF cellColor =
                        GetColorFromFramework(
                            currentGrid[gridX][y].color);

                    Graphics::DrawImage(
                        drawX,
                        drawY,
                        cellSize,
                        cellSize,
                        baseTex,
                        cellColor);
                }
            }
        }
    }

    // 各列のブロック色を描画
    float resultBarY =
        puzzleStartY +
        GRID_HEIGHT * (cellSize + cellSpacing) +
        12.0f;

    float subBoxSize =
        cellSize / 3.0f - 2.0f;

    const int currentTick =
        tickManager.GetCurrentTick();

    for (int x = 0; x < GRID_WIDTH; x++)
    {
        float drawX =
            puzzleStartX +
            x * (cellSize + cellSpacing);

        if (currentPhase == GamePhase::Playing &&
            x + 1 == currentTick)
        {
            Graphics::DrawImage(
                drawX - 3.0f,
                resultBarY - 3.0f,
                cellSize + 6.0f,
                (subBoxSize + 2.0f) * 3.0f + 6.0f,
                baseTex,
                Color::Cyan);
        }

        int blockCount = 0;
        bool displayedColors[7] = {};

        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            if (currentGrid[x][y].hasBlock)
            {
                FrameworkColor frameworkColor =
                    currentGrid[x][y].color;
                int colorIndex =
                    static_cast<int>(frameworkColor);

                if (colorIndex >= 0 &&
                    colorIndex < 7 &&
                    displayedColors[colorIndex])
                {
                    continue;
                }

                if (colorIndex >= 0 && colorIndex < 7)
                {
                    displayedColors[colorIndex] = true;
                }

                ColorF blockColor =
                    GetColorFromFramework(frameworkColor);

                float subY =
                    resultBarY +
                    blockCount * (subBoxSize + 2.0f);

                Graphics::DrawImage(
                    drawX,
                    subY,
                    cellSize,
                    subBoxSize,
                    baseTex,
                    blockColor);

                blockCount++;
            }
        }

        if (blockCount == 0)
        {
            Graphics::DrawImage(
                drawX,
                resultBarY,
                cellSize,
                18.0f,
                baseTex,
                Color::Gray);
        }
    }

    // コンソールへの配置状態表示
    if (currentPhase == GamePhase::Setup &&
        needsRedraw)
    {
        system("cls");

        cout << "=== 【フェーズ: パズル配置】 ===\n";
        simulator.DrawGrid();

        if (heldIdx != -1)
        {
            cout << "\n状態: 【ブロック移動中】 "
                    "(スペースキーで配置)\n";
        }
        else
        {
            cout << "\n状態: 【ブロック選択中】 "
                    "(W/A/S/Dで選択, スペースで掴む)\n";
        }

        cout << "操作: [W/A/S/D]移動 "
                "[Space]決定/解除 "
                "[R]リセット "
                "[Enter]プレイ開始\n";

        needsRedraw = false;
    }

    if (resultShown)
    {
        DrawResultOverlay();
    }
}

void GameScene::DrawResultOverlay()
{
    const int collectedCount =
        (stageStarCollected ? 1 : 0) +
        (clearStarCollected ? 1 : 0) +
        (tickStarCollected ? 1 : 0);

    const float panelX = 300.0f;
    const float panelY = 120.0f;
    const float panelWidth = 680.0f;
    const float panelHeight = 440.0f;

    // 背景パネル
    Graphics::DrawRect(
        panelX,
        panelY,
        panelWidth,
        panelHeight,
        ColorF(0.08f, 0.08f, 0.12f, 0.95f));

    // パネルの枠
    const float border = 8.0f;

    Graphics::DrawRect(
        panelX,
        panelY,
        panelWidth,
        border,
        Color::Yellow);

    Graphics::DrawRect(
        panelX,
        panelY + panelHeight - border,
        panelWidth,
        border,
        Color::Yellow);

    Graphics::DrawRect(
        panelX,
        panelY,
        border,
        panelHeight,
        Color::Yellow);

    Graphics::DrawRect(
        panelX + panelWidth - border,
        panelY,
        border,
        panelHeight,
        Color::Yellow);

    // タイトルバー
    Graphics::DrawRect(
        panelX + 150.0f,
        panelY + 45.0f,
        380.0f,
        18.0f,
        Color::White);

    // 3種類のスターを表示
    const float starSize = 64.0f;
    const float starY = panelY + 145.0f;
    const float firstStarX = panelX + 125.0f;

    const bool stars[3] = {
        stageStarCollected,
        clearStarCollected,
        tickStarCollected
    };

    for (int i = 0; i < 3; i++)
    {
        const ColorF starColor =
            stars[i]
                ? Color::Yellow
                : ColorF(
                    0.25f,
                    0.25f,
                    0.25f,
                    1.0f);

        Graphics::DrawImage(
            firstStarX + i * 150.0f,
            starY,
            starSize,
            starSize,
            texWhite,
            starColor);
    }

    // 獲得数 / 3
    DrawDigit(
        collectedCount,
        panelX + 245.0f,
        panelY + 285.0f,
        52.0f,
        Color::Yellow);

    Graphics::DrawRect(
        panelX + 345.0f,
        panelY + 310.0f,
        40.0f,
        7.0f,
        Color::White);

    DrawDigit(
        3,
        panelX + 425.0f,
        panelY + 285.0f,
        52.0f,
        Color::White);

    // スペースキー待機表示
    Graphics::DrawRect(
        panelX + 120.0f,
        panelY + 385.0f,
        440.0f,
        18.0f,
        Color::Green);
}