#pragma once
#pragma once

#include "BlockData.h"
#include "CSVLoad.h"
#include "Input.h"
#include <vector>

struct PlacedBlock {
    int id;
    int x, y;
    int sourceIndex;
    FrameworkColor blockColor;
    bool isPlaced;
    int shape[3][3]; // ブロックごとの形（1:ブロックあり, 0:なし）
};

class PlacementSimulator {
private:
    std::vector<PlacedBlock> blocks;
    // CSVから取得したブロック。sourceAvailableがtrueの間は右下に表示される。
    std::vector<PlacedBlock> sourceBlocks;
    int focusedBlockIndex;
    int heldBlockIndex;
    int sourceFocusIndex;
    bool focusIsSource;
    std::vector<bool> sourceAvailable;
    // WASD長押し時の連続移動間隔を調整するカウンター。
    int inputRepeatCounter = 0;

    FrameworkColor stageFixedGimmickMap[GRID_WIDTH][GRID_HEIGHT];
    bool stageWallMap[GRID_WIDTH][GRID_HEIGHT];

    bool CheckCollision(int blockIndex, int testX, int testY) const;
    // 右上・右下を共通の選択候補として、方向に最も近いブロックへ移動する。
    void MoveSelection(int dx, int dy);

public:
    PlacementSimulator();

    void MoveFocus(int dx, int dy);
    void ToggleGrab();
    void MoveHeldBlock(int dx, int dy);
    // 移動中のブロックをCSV側へ戻す（Cキーから呼び出す）。
    bool ReturnHeldBlockToSource();
    bool UpdateInput()
    {
        bool moved = false;
        bool hasDirectionalInput =
            Input::IsKeyHeld(Key::W) ||
            Input::IsKeyHeld(Key::S) ||
            Input::IsKeyHeld(Key::A) ||
            Input::IsKeyHeld(Key::D) ||
            Input::IsKeyHeld(Key::Up) ||
            Input::IsKeyHeld(Key::Down) ||
            Input::IsKeyHeld(Key::Left) ||
            Input::IsKeyHeld(Key::Right);

        if (!hasDirectionalInput)
        {
            inputRepeatCounter = 0;
            return false;
        }

        bool canMove = inputRepeatCounter == 0 || inputRepeatCounter >= 6;
        inputRepeatCounter++;

        if (!canMove)
        {
            return false;
        }

        inputRepeatCounter = 1;

        if (Input::IsKeyHeld(Key::W) || Input::IsKeyHeld(Key::Up))
        {
            if (heldBlockIndex != -1)
            {
                MoveHeldBlock(0, -1);
            }
            else
            {
                MoveSelection(0, -1);
            }
            moved = true;
        }

        if (Input::IsKeyHeld(Key::S) || Input::IsKeyHeld(Key::Down))
        {
            if (heldBlockIndex != -1)
            {
                MoveHeldBlock(0, 1);
            }
            else
            {
                MoveSelection(0, 1);
            }
            moved = true;
        }

        if (Input::IsKeyHeld(Key::A) || Input::IsKeyHeld(Key::Left))
        {
            if (heldBlockIndex != -1)
            {
                MoveHeldBlock(-1, 0);
            }
            else
            {
                MoveSelection(-1, 0);
            }
            moved = true;
        }

        if (Input::IsKeyHeld(Key::D) || Input::IsKeyHeld(Key::Right))
        {
            if (heldBlockIndex != -1)
            {
                MoveHeldBlock(1, 0);
            }
            else
            {
                MoveSelection(1, 0);
            }
            moved = true;
        }

        return moved;
    }
    void ResetBlocks();

    int GetFocusedIndex() const { return focusedBlockIndex; }
    int GetHeldIndex() const { return heldBlockIndex; }
    int GetSourceFocusIndex() const { return sourceFocusIndex; }
    bool IsSourceFocused() const { return focusIsSource; }
    bool IsSourceBlockAvailable(int index) const
    {
        return index >= 0 &&
            index < static_cast<int>(sourceAvailable.size()) &&
            sourceAvailable[index];
    }

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