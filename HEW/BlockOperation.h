#pragma once
#include "CSVLoad.h"
#include "contents.h"

class BlockOperation
{
	//Object m_player; // プレイヤーのオブジェクト
	//Object m_background; // 背景のオブジェクト
	//Object m_upperMovingblock[upperWidth][upperHeight];
	//Object m_upperFixingblock[upperWidth][upperHeight];
	//Object m_upperSelectingblock[upperWidth][upperHeight];
	//Object m_lowerSelectingblock[lowerWidth][lowerHeight];
	//Object m_lowerNonselectingblocks[lowerWidth][lowerHeight];

	CSVLoad blockdata;

	int m_upperdata[upperWidth][upperHeight][upperBlockState] = {};
	int m_lowerdata[lowerWidth][lowerHeight][upperBlockState] = {};

	//初期化したいblockと場所、サイズを入れる
	//void InitBlocks(Object& block, float posx, float posy, int size);

	static constexpr int MAXBLOCK = 6;
	int blockNum = 0;
	int startX = 0;
	int startY = 0;

	//下のブロックで選ばれてるブロック
	int blockList[blocklistWidth][blocklistHeight] = {};

	//上画面で設置されてるブロックを格納する配列
	int SetedBlocknum[MAXBLOCK] = {};
	int num = 0;
	int deleteX = 0;
	int deleteY = 0;

	enum class Color
	{
		none,
		red,
		blue,
		yellow,
		rightred,
		rightblue,
		rightyellow,
		black,
	};

	Color color = Color::none;

	enum class Phase
	{
		LowerBlockSelect,
		UpperDecideBlockPlace,
		UpperBlockSelect
	};

	Phase currentPhase = Phase::LowerBlockSelect;

	void upperSelectBlock();
	void upperSelectMove(int x, int y);

	void lowerSelectBlock();
	void lowerMove(int x, int y);
	//上にブロックを出す
	void PutBlock();

	//動かせれるか
	int IsInvalidBlockMove(int x, int y, int state);

	void moveBlock();
	void moveBlockX(int x);
	void moveBlockY(int y);

	//ブロックの移動
	void ShiftBlock(int x, int y, int state);

	//上の画面でブロックが外に出た時
	void OutofBlock();

	//置けるのか
	bool IsInvalidBlockSet();

	//上のブロックの場所を決める
	bool SetBlock();

	int RecordBlock = 0;

	int restoreX = 0;
	int restoreY = 0;

	//リセットできるはず
	void ResetBlocks();
public:
	// 初期化処理
	void Init();

	// ゲーム更新処理
	void Update();
	//void SetColor();
	// 描画処理
	void Draw();

	// 解放処理
	void Uninit();

	bool isSelect = false;

	int GetCurrentPhase();

	bool isMove = false;
	bool isSet = false;
};

