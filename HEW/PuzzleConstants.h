#pragma once
#include "Graphics.h"
#include "BlockData.h"

// ==========================================
// パズル・UIに関する定数および共通ヘルパー
// ==========================================
namespace PuzzleConstants
{
	constexpr float CellSize = 65.0f;
	constexpr float CellSpacing = 6.0f;
	constexpr float RightMargin = 25.0f;
	constexpr float FrameWidth = 4.0f;
	constexpr float WindowWidth = 1280.0f;
   constexpr float WorldWidth = 2560.0f;
	constexpr float PuzzleStartY = 40.0f;

	// ステージスターの位置とサイズ
	constexpr float StageStarX = 300.0f;
    constexpr float StageStarY = 610.0f;
	constexpr float StageStarSize = 40.0f;

	// パズルスターの配置マス
	constexpr int PuzzleStarTargetX = 2;
	constexpr int PuzzleStarTargetY = 1;
	constexpr float PuzzleStarSize = 36.0f;

	// ゴールオブジェクト
	constexpr float GoalX = 760.0f;
 constexpr float GoalY = 550.0f;
	constexpr float GoalWidth = 70.0f;
	constexpr float GoalHeight = 100.0f;
}

// FrameworkColorをDirectXのColorFに変換する共通ヘルパー関数
inline ColorF GetColorFromFramework(FrameworkColor fColor)
{
	switch (fColor)
	{
	case FrameworkColor::Red:    return Color::Red;
	case FrameworkColor::Blue:   return Color::Blue;
	case FrameworkColor::Yellow: return Color::Yellow;
	case FrameworkColor::Purple: return Color::Purple;
	case FrameworkColor::Orange: return Color::Orange;
	case FrameworkColor::Green:  return Color::Green;
	default:                     return Color::White;
	}
}