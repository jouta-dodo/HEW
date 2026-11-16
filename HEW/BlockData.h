#pragma once

enum class FrameworkColor {
    None = 0,
    Red = 1,
    Blue = 2,
    Yellow = 3,
    Purple = 4,   // 赤1 + 青ギミック2
    Orange = 5,   // 赤1 + 黄ギミック3
    Green = 6     // 青2 + 黄ギミック3
};

struct BlockCell {
    FrameworkColor color = FrameworkColor::None;
    bool hasBlock = false;
};

const int GRID_WIDTH = 5;
const int GRID_HEIGHT = 3;

enum class GamePhase {
    Setup,   // パズル・配置フェーズ
    Playing  // 操作・アクションフェーズ
};