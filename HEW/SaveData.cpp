/**
 * @file SaveData.cpp
 * @brief SaveDataクラスのメンバ関数実装
 */

#include "SaveData.h"
#include <fstream>
#include <iostream>

 /**
  * @brief コンストラクタ
  */
SaveData::SaveData() {
    // 予定している最大ステージ数に合わせてベクターのサイズを確保
    stageDataList.resize(maxStages);
}

void SaveData::ResetAll()
{
    for (int i = 0; i < maxStages; i++)
    {
        stageDataList[i] = StageSaveData{};
    }
}

/**
 * @brief 指定ステージのデータを取得
 */
StageSaveData& SaveData::GetStageData(int stageIndex) {
    // 範囲外アクセスを防ぐためのガード処理
    if (stageIndex < 0 || stageIndex >= maxStages) {
        std::cout << "[SaveData] 警告: 無効なステージインデックスが指定されました: " << stageIndex << "\n";
        static StageSaveData dummy; // 範囲外の場合はダミーを返す
        return dummy;
    }
    return stageDataList[stageIndex];
}

/**
 * @brief セーブ処理（テキスト形式でファイル出力）
 * フォーマット: [クリアフラグ] [スター1] [スター2] [スター3] [総スター数]
 */
bool SaveData::Save() {
    std::ofstream file(saveFileName);
    if (!file.is_open()) {
        std::cout << "[SaveData] エラー: セーブファイルを開けませんでした（書き込み失敗）。\n";
        return false;
    }

    // 各ステージのデータを1行ずつスペース区切りで書き込み
    for (int i = 0; i < maxStages; i++) {
        file << stageDataList[i].isCleared << " "
            << stageDataList[i].star1_action << " "
            << stageDataList[i].star2_puzzle << " "
            << stageDataList[i].star3_time << " "
            << stageDataList[i].earnedStars << "\n";
    }

    file.close();
    std::cout << "[SaveData] セーブが正常に完了しました。\n";
    return true;
}

/**
 * @brief ロード処理（テキストファイルからの読み込み）
 */
bool SaveData::Load() {
    std::ifstream file(saveFileName);
    if (!file.is_open()) {
        std::cout << "[SaveData] 情報: セーブファイルが存在しません。初期状態から開始します。\n";
        return false;
    }

    // ファイルから1行ずつデータを読み込む
    for (int i = 0; i < maxStages; i++) {
        if (!(file >> stageDataList[i].isCleared
            >> stageDataList[i].star1_action
            >> stageDataList[i].star2_puzzle
            >> stageDataList[i].star3_time
            >> stageDataList[i].earnedStars)) {
            break; // ファイルの終端に達した場合はループを抜け、既存データを維持
        }
    }

    file.close();
    std::cout << "[SaveData] セーブデータのロードが正常に完了しました。\n";
    return true;
}

/**
 * @brief デバッグ用：セーブ状況のコンソール出力
 */
void SaveData::PrintDebug() const {
    std::cout << "========== 【 SaveData Debug Info 】 ==========\n";
    for (int i = 0; i < maxStages; i++) {
        std::cout << " ステージ " << (i + 1)
            << " | クリア: " << (stageDataList[i].isCleared ? "〇" : "×")
            << " | 獲得スター: " << stageDataList[i].earnedStars << "/3"
            << " [内訳 - アクション:" << stageDataList[i].star1_action
            << ", パズル空白:" << stageDataList[i].star2_puzzle
            << ", 制限時間:" << stageDataList[i].star3_time << "]\n";
    }
    std::cout << "===============================================\n";
}