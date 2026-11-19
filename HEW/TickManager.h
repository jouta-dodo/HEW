#pragma once

#include "CountJudgeMock.h"

// ティックの進行と、現在の列の判定を管理するクラス。
//
// 使い方:
// 1. GameSceneのメンバー変数として宣言する
//      TickManager tickManager;
//
// 2. プレイ開始時に初期化する
//      tickManager.Reset();
//
// 3. Playingフェーズ中に毎フレーム呼び出す
//      tickManager.Update(1.0f / 60.0f, judgeSystem);
//
// Update()を呼び出すと、一定時間ごとに判定対象の列が進み、
// CountJudgeMock::EvaluateColumn()が自動的に呼び出される。
class TickManager
{
private:
    int currentTick = 1;
    int tickLoopCount = 1;

    float tickTimer = 0.0f;
    const float tickInterval = 0.5f;

public:
    // ティック番号、タイマー、ループ回数を初期状態へ戻す。
    void Reset();

    // ティックを更新し、現在の列を判定する。
    void Update(float deltaTime, CountJudgeMock& judgeSystem);

    // 現在判定している列番号を取得する。
    int GetCurrentTick() const;

    // 経過したティックループ数を取得する。
    int GetTickLoopCount() const;
};