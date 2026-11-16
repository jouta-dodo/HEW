#pragma once
#include "BlockData.h"

class CountJudgeMock {
private:
    BlockCell boardGrid[GRID_WIDTH][GRID_HEIGHT];

public:
    CountJudgeMock();

    void SetCompletedGrid(const BlockCell inputGrid[GRID_WIDTH][GRID_HEIGHT]);
    void EvaluateColumn(int tick) const;
    BlockCell GetActiveCell(int y) const;
};