#include "CSVLoad.h"

bool CSVLoad::LoadBlockCSV(const char* fileName)
{
	std::ifstream file(fileName, std::ios::binary);

	if (!file.is_open())
	{
		OutputDebugStringA(
			"CSVファイルを開けませんでした。\n"
		);
		return false;
	}

	std::string line;
	int row = 0;

	while (std::getline(file, line))
	{
		// Windowsの改行コードに含まれる\rを消す
		if (!line.empty() && line.back() == '\r')
		{
			line.pop_back();
		}

		// 空行は読み飛ばす
		if (line.empty())
		{
			continue;
		}

		if (row >= lowerHeight)
		{
			OutputDebugStringA(
				"CSVの行数が11行を超えています。\n"
			);
			return false;
		}

		std::stringstream lineStream(line);
		std::string value;
		int column = 0;

		while (std::getline(lineStream, value, ','))
		{
			if (column >= lowerWidth)
			{
				OutputDebugStringA(
					"CSVの列数が7列を超えています。\n"
				);
				return false;
			}

			// CSV先頭にあるUTF-8 BOMを削除する
			if (row == 0 && column == 0 &&
				value.size() >= 3 &&
				static_cast<unsigned char>(value[0]) == 0xEF &&
				static_cast<unsigned char>(value[1]) == 0xBB &&
				static_cast<unsigned char>(value[2]) == 0xBF)
			{
				value.erase(0, 3);
			}

			// 空白・タブ・改行文字を削除
			const size_t first =
				value.find_first_not_of(" \t\r\n");

			const size_t last =
				value.find_last_not_of(" \t\r\n");

			if (first == std::string::npos)
			{
				OutputDebugStringA(
					"CSVに空のデータがあります。\n"
				);
				return false;
			}

			value = value.substr(
				first,
				last - first + 1
			);

			try
			{
				// 数字を変換した位置を保存する
				size_t convertedLength = 0;

				int number = std::stoi(
					value,
					&convertedLength
				);

				// 値の途中や末尾に数字以外がないか確認
				if (convertedLength != value.size())
				{
					std::string message =
						"CSVに数字以外の文字があります。"
						" 行=" + std::to_string(row + 1) +
						" 列=" + std::to_string(column + 1) +
						" 値=[" + value + "]\n";

					OutputDebugStringA(message.c_str());
					return false;
				}

				// 今回使用できる値は0から3
				if (number < 0 || number > 3)
				{
					std::string message =
						"CSVの値が0から3の範囲外です。"
						" 行=" + std::to_string(row + 1) +
						" 列=" + std::to_string(column + 1) +
						" 値=" + std::to_string(number) +
						"\n";

					OutputDebugStringA(message.c_str());
					return false;
				}

				// 7行11列の固定配列へ代入
				blockdata[row][column] = number;
			}
			catch (const std::exception&)
			{
				std::string message =
					"CSVを数字に変換できません。"
					" 行=" + std::to_string(row + 1) +
					" 列=" + std::to_string(column + 1) +
					" 値=[" + value + "]\n";

				OutputDebugStringA(message.c_str());
				return false;
			}

			column++;
		}

		if (column != lowerWidth)
		{
			std::string message =
				"CSVの列数が11列ではありません。"
				" 行=" + std::to_string(row + 1) +
				" 実際の列数=" + std::to_string(column) +
				"\n";

			OutputDebugStringA(message.c_str());
			return false;
		}

		row++;
	}

	if (row != lowerHeight)
	{
		std::string message =
			"CSVの行数が7行ではありません。"
			" 実際の行数=" + std::to_string(row) +
			"\n";

		OutputDebugStringA(message.c_str());
		return false;
	}

	OutputDebugStringA(
		"CSVを7x11配列へ正常に読み込みました。\n"
	);

	return true;
}

int CSVLoad::GetBlockData(int y, int x)
{
	return blockdata[y][x];
}
