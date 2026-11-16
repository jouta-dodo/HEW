#pragma once
#include "BlockData.h"
#include <vector>

struct PlacedBlock {
    int id;
    int x, y;
    FrameworkColor blockColor;
    bool isPlaced;
    int shape[3][3]; // ブロックごとの形（1:ブロックあり, 0:なし）
};

class PlacementSimulator {
private:
    std::vector<PlacedBlock> blocks;
    int focusedBlockIndex;
    int heldBlockIndex;

    FrameworkColor stageFixedGimmickMap[GRID_WIDTH][GRID_HEIGHT];
    bool stageWallMap[GRID_WIDTH][GRID_HEIGHT];

    bool CheckCollision(int blockIndex, int testX, int testY) const;

public:
    PlacementSimulator();

    void MoveFocus(int dx, int dy);
    void ToggleGrab();
    void MoveHeldBlock(int dx, int dy);
    void ResetBlocks();

    int GetFocusedIndex() const { return focusedBlockIndex; }
    int GetHeldIndex() const { return heldBlockIndex; }

    void DrawGrid() const;
    void GetCurrentGridData(BlockCell outGrid[GRID_WIDTH][GRID_HEIGHT]) const;

    int GetFocusedBlockIndex() const { return focusedBlockIndex; }
    bool IsWall(int x, int y) const { return stageWallMap[x][y]; }
    FrameworkColor GetFixedGimmick(int x, int y) const { return stageFixedGimmickMap[x][y]; }
    const std::vector<PlacedBlock>& GetBlocks() const { return blocks; }

    //「スター2（指定マスが空白か）」の判定関数
    bool CheckPuzzleStarCondition(int targetX, int targetY) const {
        BlockCell currentGrid[GRID_WIDTH][GRID_HEIGHT];
        GetCurrentGridData(currentGrid);

        if (targetX >= 0 && targetX < GRID_WIDTH && targetY >= 0 && targetY < GRID_HEIGHT) {
            return !currentGrid[targetX][targetY].hasBlock;
        }
        return true;
    }
};