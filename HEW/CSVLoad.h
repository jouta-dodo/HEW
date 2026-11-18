#pragma once

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <DirectXMath.h>
#include <Windows.h>
#include <array>
#include "contents.h"

class CSVLoad
{
public:
	using BlockData = std::array<std::array<int, lowerWidth>, lowerHeight>;
	//読み込んでくれる
	//引数のところに読み込んでほしいCSVファイルを書く
	bool LoadBlockCSV(const char* filename);

	//csvのデータをとれる（引数の順番に気を付ける(y,x)）
	//下の画面にデータを入れるときに使う
	int GetBlockData(int y, int x);
private:
	BlockData blockdata{};

};
