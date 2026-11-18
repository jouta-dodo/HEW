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
	bool LoadBlockCSV(const char* filename);

	//csvのデータをとれる（引数の順番に気を付ける(y,x)）
	int GetBlockData(int y, int x);
private:
	BlockData blockdata{};

};
