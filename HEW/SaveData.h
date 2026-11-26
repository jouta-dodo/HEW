
#pragma once
#include <string>
#include <vector>

//ステージごとのクリア状況およびスター獲得状況を保持する構造体
struct StageSaveData {
    bool isCleared = false;    ///< ステージクリアフラグ (true: クリア済)
    bool star1_action = false; ///< スター1: アクションステージ内で獲得したか
    bool star2_puzzle = false; ///< スター2: パズル指定マスが空白の状態でクリアしたか
    bool star3_time = false;   ///< スター3: 制限時間内にクリアしたか
    int earnedStars = 0;       ///< そのステージで獲得したスターの総数 (0 〜 3)
};


//セーブデータのロード、セーブ、および各ステージのデータ管理を行うクラス
class SaveData
{
private:
    std::string saveFileName = "savedata.txt"; ///< 保存先ファイル名（テキスト形式）
    std::vector<StageSaveData> stageDataList;  ///< 全ステージ分のセーブデータリスト
    const int maxStages = 10;                  ///< 管理する最大ステージ数

public:
    //コンストラクタ（最大ステージ数分のデータを初期化）

    SaveData();


    //指定したステージのセーブデータを取得する（書き換え可能な参照を返す）
    //stageIndex ステージ番号（0始まり / 0がステージ1）
    //StageSaveData& 対象ステージのセーブデータへの参照

    StageSaveData& GetStageData(int stageIndex);

    //現在の全ステージのデータをファイルに書き込む（セーブ）
    //return bool 成功した場合はtrue、失敗した場合はfalse
    bool Save();

    // 全ステージのクリア・スター進捗を初期化する。
    void ResetAll();


    //ファイルから全ステージのデータを読み込む（ロード）
    //return bool 成功した場合はtrue、ファイルが存在しない等の場合はfalse

    bool Load();


    //デバッグ用：現在の全ステージのセーブ状況をコンソールに出力する
    void PrintDebug() const;
};