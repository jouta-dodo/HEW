#include "PlacementSimulator.h"
#include "PlacementSimulator.h"
#include <iostream>
#include <cmath>

PlacementSimulator::PlacementSimulator()
    : focusedBlockIndex(-1),
      heldBlockIndex(-1),
      sourceFocusIndex(0),
      focusIsSource(true)
{
    // テスト用ブロックは作らず、stage1.csvの6個の3×3ブロックを使用する。
    //固定ギミックと壁マップを初期化
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            stageFixedGimmickMap[x][y] = FrameworkColor::None;
            stageWallMap[x][y] = false;
        }
    }

    // 固定ギミック・壁の配置のエリア
    stageFixedGimmickMap[2][2] = FrameworkColor::Blue; //青ギミック
    stageWallMap[1][1] = true;                         //1マスの壁

    CSVLoad csvData;
    csvData.LoadBlockCSV("assets/stage1.csv");

    for (int sourceIndex = 0; sourceIndex < 6; sourceIndex++)
    {
        PlacedBlock sourceBlock{};
        sourceBlock.id = sourceIndex + 1;
        sourceBlock.x = 0;
        sourceBlock.y = 0;
        sourceBlock.sourceIndex = sourceIndex;
        sourceBlock.isPlaced = false;

        int startX = (sourceIndex % 3) * 4;
        int startY = (sourceIndex / 3) * 4;
        FrameworkColor sourceColor = FrameworkColor::None;

        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 3; x++)
            {
                int value = csvData.GetBlockData(startY + y, startX + x);
                sourceBlock.shape[y][x] = (value != 0) ? 1 : 0;

                if (sourceColor == FrameworkColor::None)
                {
                    if (value == 1) sourceColor = FrameworkColor::Red;
                    else if (value == 2) sourceColor = FrameworkColor::Blue;
                    else if (value == 3) sourceColor = FrameworkColor::Yellow;
                }

            }
        }

        sourceBlock.blockColor = sourceColor;
        sourceBlocks.push_back(sourceBlock);
        sourceAvailable.push_back(true);
    }
}

bool PlacementSimulator::ReturnHeldBlockToSource()
{
    if (heldBlockIndex < 0 ||
        heldBlockIndex >= static_cast<int>(blocks.size()))
    {
        return false;
    }

    const int returnedSourceIndex =
        blocks[heldBlockIndex].sourceIndex;

    if (returnedSourceIndex < 0 || returnedSourceIndex >= 6)
    {
        return false;
    }

    blocks.erase(blocks.begin() + heldBlockIndex);
    sourceAvailable[returnedSourceIndex] = true;
    sourceFocusIndex = returnedSourceIndex;
    focusedBlockIndex = -1;
    heldBlockIndex = -1;
    focusIsSource = true;
    return true;
}

void PlacementSimulator::MoveSelection(int dx, int dy)
{
    int currentX = 0;
    int currentY = 0;

    if (focusIsSource)
    {
        currentX = (sourceFocusIndex % 3) * 4 + 1;
        currentY = 4 + (sourceFocusIndex / 3) * 4 + 1;
    }
    else if (focusedBlockIndex >= 0 &&
        focusedBlockIndex < static_cast<int>(blocks.size()))
    {
        currentX = blocks[focusedBlockIndex].x + 1;
        currentY = blocks[focusedBlockIndex].y + 1;
    }
    else
    {
        focusIsSource = true;
        sourceFocusIndex = 0;
        return;
    }

    int nextIndex = -1;
    bool nextIsSource = false;
    double minDistance = 9999.0;

    for (size_t i = 0; i < blocks.size(); i++)
    {
        int candidateX = blocks[i].x + 1;
        int candidateY = blocks[i].y + 1;
        int diffX = candidateX - currentX;
        int diffY = candidateY - currentY;

        if ((dx != 0 && diffX * dx <= 0) ||
            (dy != 0 && diffY * dy <= 0))
        {
            continue;
        }

        double distance = std::abs(diffX) + std::abs(diffY);
        if (distance < minDistance)
        {
            minDistance = distance;
            nextIndex = static_cast<int>(i);
            nextIsSource = false;
        }
    }

    for (size_t i = 0; i < sourceBlocks.size(); i++)
    {
        if (!sourceAvailable[i]) continue;

        int candidateX = static_cast<int>(i % 3) * 4 + 1;
        int candidateY = 4 + static_cast<int>(i / 3) * 4 + 1;
        int diffX = candidateX - currentX;
        int diffY = candidateY - currentY;

        if ((dx != 0 && diffX * dx <= 0) ||
            (dy != 0 && diffY * dy <= 0))
        {
            continue;
        }

        double distance = std::abs(diffX) + std::abs(diffY);
        if (distance < minDistance)
        {
            minDistance = distance;
            nextIndex = static_cast<int>(i);
            nextIsSource = true;
        }
    }

    if (nextIndex == -1) return;

    if (nextIsSource)
    {
        sourceFocusIndex = nextIndex;
        focusIsSource = true;
    }
    else
    {
        focusedBlockIndex = nextIndex;
        focusIsSource = false;
    }
}

//置く瞬間の重なりチェック3×3の「1」の部分だけで判定
bool PlacementSimulator::CheckCollision(int blockIndex, int testX, int testY) const
{
    const auto& target = blocks[blockIndex];

    for (int ly = 0; ly < 3; ly++)
    {
        for (int lx = 0; lx < 3; lx++)
        {
            if (target.shape[ly][lx] == 0) continue; //0の場所は当たり判定なし

            int gx = testX + lx;
            int gy = testY + ly;

            // 1. 壁との衝突チェック
            if (gx >= 0 && gx < GRID_WIDTH && gy >= 0 && gy < GRID_HEIGHT)
            {
                if (stageWallMap[gx][gy]) return true;
            }

            //他のブロックとの衝突チェック
            for (size_t i = 0; i < blocks.size(); i++)
            {
                if ((int)i == blockIndex) continue;
                const auto& other = blocks[i];

                for (int oly = 0; oly < 3; oly++)
                {
                    for (int olx = 0; olx < 3; olx++)
                    {
                        if (other.shape[oly][olx] == 0) continue;

                        int ogx = other.x + olx;
                        int ogy = other.y + oly;
                        if (gx == ogx && gy == ogy) return true;
                    }
                }
            }
        }
    }
    return false;
}

void PlacementSimulator::MoveFocus(int dx, int dy)
{
    if (heldBlockIndex != -1) return;

    double minDist = 999.0;
    int nextIndex = focusedBlockIndex;

    for (size_t i = 0; i < blocks.size(); i++)
    {
        if ((int)i == focusedBlockIndex) continue;

        int diffX = blocks[i].x - blocks[focusedBlockIndex].x;
        int diffY = blocks[i].y - blocks[focusedBlockIndex].y;

        if (dx != 0 && diffX * dx > 0) {
            double dist = std::abs(diffX) + std::abs(diffY);
            if (dist < minDist) { minDist = dist; nextIndex = i; }
        }
        else if (dy != 0 && diffY * dy > 0) {
            double dist = std::abs(diffX) + std::abs(diffY);
            if (dist < minDist) { minDist = dist; nextIndex = i; }
        }
    }
    focusedBlockIndex = nextIndex;
}

void PlacementSimulator::ToggleGrab()
{
    if (heldBlockIndex == -1)
    {
        if (focusIsSource)
        {
            // 右下のCSVブロックを右上へ取得する。
            if (sourceBlocks.empty() ||
                sourceFocusIndex < 0 ||
                sourceFocusIndex >= static_cast<int>(sourceBlocks.size()) ||
                !sourceAvailable[sourceFocusIndex])
            {
                return;
            }

            PlacedBlock selectedBlock = sourceBlocks[sourceFocusIndex];
            selectedBlock.id = static_cast<int>(blocks.size()) + 1;
            selectedBlock.x = 0;
            selectedBlock.y = 0;
            selectedBlock.isPlaced = false;
            blocks.push_back(selectedBlock);
            sourceAvailable[sourceFocusIndex] = false;
            focusedBlockIndex = static_cast<int>(blocks.size()) - 1;
            focusIsSource = false;
            heldBlockIndex = focusedBlockIndex;
        }
        else if (focusedBlockIndex >= 0 &&
            focusedBlockIndex < static_cast<int>(blocks.size()))
        {
            heldBlockIndex = focusedBlockIndex;
        }
    }
    else {
        // 右上で移動中のブロックをSpaceで配置確定する。
        if (!CheckCollision(heldBlockIndex, blocks[heldBlockIndex].x, blocks[heldBlockIndex].y))
        {
            blocks[heldBlockIndex].isPlaced = true;
            heldBlockIndex = -1;
        }
        else
        {
            std::cout << "\n壁または他のブロックと重なっているため置けません！\n";
        }
    }
}

void PlacementSimulator::MoveHeldBlock(int dx, int dy) {
    if (heldBlockIndex == -1) return;

    int nextX = blocks[heldBlockIndex].x + dx;
    int nextY = blocks[heldBlockIndex].y + dy;

    //ブロックの実体 1 が盤面5×3からはみ出さないように制限
    bool outOfBounds = false;
    for (int ly = 0; ly < 3; ly++)
    {
        for (int lx = 0; lx < 3; lx++)
        {
            if (blocks[heldBlockIndex].shape[ly][lx] == 1)
            {
                int gx = nextX + lx;
                int gy = nextY + ly;
                if (gx < 0 || gx >= GRID_WIDTH || gy < 0 || gy >= GRID_HEIGHT)
                {
                    outOfBounds = true;
                }
            }
        }
    }

    if (!outOfBounds)
    {
        blocks[heldBlockIndex].x = nextX;
        blocks[heldBlockIndex].y = nextY;
    }
}

void PlacementSimulator::ResetBlocks()
{
    blocks.clear();
    focusedBlockIndex = -1;
    heldBlockIndex = -1;
    sourceFocusIndex = 0;
    focusIsSource = true;
    inputRepeatCounter = 0;
    for (size_t i = 0; i < sourceAvailable.size(); i++)
    {
        sourceAvailable[i] = true;
    }
}

void PlacementSimulator::DrawGrid() const
{
    char display[GRID_HEIGHT][GRID_WIDTH];

    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            if (stageWallMap[x][y])
            {
                display[y][x] = '#';
            }
            else if (stageFixedGimmickMap[x][y] == FrameworkColor::Blue)
            {
                display[y][x] = 'g';
            }
            else {
                display[y][x] = '.';
            }
        }
    }

    for (size_t i = 0; i < blocks.size(); i++)
    {
        char symbol = '1' + i;
        for (int ly = 0; ly < 3; ly++)
        {
            for (int lx = 0; lx < 3; lx++)
            {
                if (blocks[i].shape[ly][lx] == 1)
                {
                    int px = blocks[i].x + lx;
                    int py = blocks[i].y + ly;
                    if (px >= 0 && px < GRID_WIDTH && py >= 0 && py < GRID_HEIGHT)
                    {
                        display[py][px] = symbol;
                    }
                }
            }
        }
    }

    std::cout << "\n  パズル配置 (1:赤, 2:黄, #:壁, g:テストで固定は青色に設定 \n";
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        std::cout << "  ";
        for (int x = 0; x < GRID_WIDTH; x++) { std::cout << display[y][x] << " "; }
        std::cout << "\n";
    }
    std::cout << " \n";

    if (heldBlockIndex != -1) {
        std::cout << " 状態:  ブロック " << (heldBlockIndex + 1) << " を移動中 壁すり抜け可能  \n";
    }
    else {
        std::cout << " 状態: ［ 選択枠: ブロック " << (focusedBlockIndex + 1) << " ］ (決定で掴む)\n";
    }
}

void PlacementSimulator::GetCurrentGridData(BlockCell outGrid[GRID_WIDTH][GRID_HEIGHT]) const
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            outGrid[x][y] = { FrameworkColor::None, false };
        }
    }

    FrameworkColor blockColorMap[GRID_WIDTH][GRID_HEIGHT] = { FrameworkColor::None };
    bool blockExist[GRID_WIDTH][GRID_HEIGHT] = { false };

    for (size_t i = 0; i < blocks.size(); i++) {
        FrameworkColor bColor = blocks[i].blockColor;
        for (int ly = 0; ly < 3; ly++) {
            for (int lx = 0; lx < 3; lx++) {
                if (blocks[i].shape[ly][lx] == 1) {
                    int px = blocks[i].x + lx;
                    int py = blocks[i].y + ly;
                    if (px >= 0 && px < GRID_WIDTH && py >= 0 && py < GRID_HEIGHT) {
                        blockExist[px][py] = true;
                        blockColorMap[px][py] = bColor;
                    }
                }
            }
        }
    }

    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            if (!blockExist[x][y]) continue;

            outGrid[x][y].hasBlock = true;
            FrameworkColor bCol = blockColorMap[x][y];
            FrameworkColor gCol = stageFixedGimmickMap[x][y];

            if (gCol != FrameworkColor::None) {
                if ((bCol == FrameworkColor::Red && gCol == FrameworkColor::Blue) ||
                    (bCol == FrameworkColor::Blue && gCol == FrameworkColor::Red))
                {
                    outGrid[x][y].color = FrameworkColor::Purple;
                }
                else if ((bCol == FrameworkColor::Red && gCol == FrameworkColor::Yellow) ||
                    (bCol == FrameworkColor::Yellow && gCol == FrameworkColor::Red))
                {
                    outGrid[x][y].color = FrameworkColor::Orange;
                }
                else if ((bCol == FrameworkColor::Blue && gCol == FrameworkColor::Yellow) ||
                    (bCol == FrameworkColor::Yellow && gCol == FrameworkColor::Blue))
                {
                    outGrid[x][y].color = FrameworkColor::Green;
                }
                else
                {
                    outGrid[x][y].color = bCol;
                }
            }
            else
            {
                outGrid[x][y].color = bCol;
            }
        }
    }
}