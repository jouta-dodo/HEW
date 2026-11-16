#include "CountJudgeMock.h"
#include <iostream>

CountJudgeMock::CountJudgeMock()
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            boardGrid[x][y] = { FrameworkColor::None, false };
        }
    }
}

void CountJudgeMock::SetCompletedGrid(const BlockCell inputGrid[GRID_WIDTH][GRID_HEIGHT]) {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            boardGrid[x][y] = inputGrid[x][y];
        }
    }
}

void CountJudgeMock::EvaluateColumn(int tick) const {
    int targetColumn = tick - 1;
    std::cout << "  [ティック " << tick << " ( " << tick << "列目 )]: ";

    bool hasAnyBlock = false;
    for (int y = 0; y < GRID_HEIGHT; y++) {
        if (boardGrid[targetColumn][y].hasBlock) {
            hasAnyBlock = true;
            int colorId = (int)boardGrid[targetColumn][y].color;

            std::string colorName = "不明";
            if (colorId == 1) colorName = "赤(1)";
            else if (colorId == 2) colorName = "青(2)";
            else if (colorId == 3) colorName = "黄(3)";
            else if (colorId == 4) colorName = "紫(4)";
            else if (colorId == 5) colorName = "橙(5)";
            else if (colorId == 6) colorName = "緑(6)";

            std::cout << "[行" << (y + 1) << ": 色ID=" << colorId << "(" << colorName << ")] ";
        }
    }
    if (!hasAnyBlock) {
        std::cout << "なし (全行OFF)";
    }
    std::cout << "\n";
}

BlockCell CountJudgeMock::GetActiveCell(int y) const {
    if (y >= 0 && y < GRID_HEIGHT) {
        return boardGrid[0][y];
    }
    return { FrameworkColor::None, false };
}