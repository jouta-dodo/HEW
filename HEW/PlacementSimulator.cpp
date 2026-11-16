#include "PlacementSimulator.h"
#include <iostream>
#include <cmath>

PlacementSimulator::PlacementSimulator() : focusedBlockIndex(0), heldBlockIndex(-1)
{
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

    // =====================================================================
    // ブロックの形を作るエリア：1がブロックの実体、0が空白
    // =====================================================================

    //赤のL字型
    PlacedBlock b1;
    b1.id = 1; b1.x = 0; b1.y = 0; b1.blockColor = FrameworkColor::Red; b1.isPlaced = true;
    int s1[3][3] =
    {
        {1, 0, 0},
        {1, 0, 0},
        {1, 1, 0}
    };
    for (int y = 0; y < 3; y++) for (int x = 0; x < 3; x++) b1.shape[y][x] = s1[y][x];
    blocks.push_back(b1);

    //横2マス型
    PlacedBlock b2;
    b2.id = 2; b2.x = 3; b2.y = 0; b2.blockColor = FrameworkColor::Yellow; b2.isPlaced = true;
    int s2[3][3] = {
        {1, 1, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    for (int y = 0; y < 3; y++) for (int x = 0; x < 3; x++) b2.shape[y][x] = s2[y][x];
    blocks.push_back(b2);
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
        heldBlockIndex = focusedBlockIndex;
    }
    else {
        if (!CheckCollision(heldBlockIndex, blocks[heldBlockIndex].x, blocks[heldBlockIndex].y))
        {
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
    blocks[0].x = 0; blocks[0].y = 0;
    blocks[1].x = 3; blocks[1].y = 0;
    focusedBlockIndex = 0;
    heldBlockIndex = -1;
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