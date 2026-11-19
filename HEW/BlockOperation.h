#pragma once
#include "CSVLoad.h"
#include "contents.h"
#include "Graphics.h"
#include <iostream>
#include <vector>
#include <array>

using upperblockdata = std::array<std::array<int, upperHeight>, upperWidth>;

enum class BlockType
{
	none,
	red,
	blue,
	yellow,
	rightred,
	rightblue,
	rightyellow,
	black,
	white
};
//
//enum class BlockState
//{
//	Enpty,
//	Selected,
//	Unselected,
//	Moving,
//	Fixing
//};

struct StringBlocks
{
	BlockType type = BlockType::none;
	//BlockState state = BlockState::Enpty;
};

class BlockOperation
{
private:
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
		white
	};

	Color color = Color::none;

	void SetColor();

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

	ComPtr<ID3D11ShaderResourceView> blocktexture;
public:
	// 初期化処理
	void Init(ComPtr<ID3D11ShaderResourceView> texture);

	// ゲーム更新処理
	void Update();
	//void SetColor();
	// 描画処理
	void Draw();

	// 解放処理
	void Uninit();

	bool isSelect = false;

	int GetUpperBlockState(int x, int y);
	int GetLowerBlockState(int x, int y);

	int GetCurrentPhase();

	//上のデータを１マスを返り値にする
	// 例）
	//int a = GetUpperBlockData(0,0);
	//にしたらaに上の画面の配列の0,0のデータが入る
	int GetUpperBlockData(int x,int y);

	//上のデータの配列全体が返り値になってる
	//upperblockdataで変数を作って、それにBlockOperationのオブジェクトからこの関数を呼び出すと変数に上の画面のデータを入れれる
	//例）
	//BlockOperation b;
	//upperblockdata data = b.GetUpperBlockData();
	//int a = data[x][y];	//←dataはx,yの順番でその配列を見れる
	upperblockdata GetUpperBlockData();

	StringBlocks m_upperblocks[upperWidth][upperHeight];
	StringBlocks m_lowerblocks[lowerWidth][lowerHeight];

	bool isMove = false;
	bool isSet = false;
};

inline ColorF GetBlockColor(BlockType type)
{
	switch (type)
	{
	case BlockType::none:		 return ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	case BlockType::red:		 return ColorF(1.0f, 0.0f, 0.0f, 1.0f);
	case BlockType::blue:		 return ColorF(0.0f, 0.0f, 1.0f, 1.0f);
	case BlockType::yellow:		 return ColorF(1.0f, 1.0f, 0.0f, 1.0f);
	case BlockType::rightred:	 return ColorF(1.0f, 0.0f, 0.0f, 0.2f);
	case BlockType::rightblue:	 return ColorF(0.0f, 0.0f, 1.0f, 0.2f);
	case BlockType::rightyellow: return ColorF(1.0f, 1.0f, 0.0f, 0.2f);
	case BlockType::black:		 return ColorF(0.0f, 0.0f, 0.0f, 0.2f);
	default:break;
	}
}