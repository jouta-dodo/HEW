#pragma once
#include<string>
class Audio
{
public:
	//音源ファイルを読み込んで名前を付ける	
	static void Load(const std::wstring& filename, const std::wstring& alias);

	//BGMとして鳴らす
	static void PlayBGM(const std::wstring& alias);

	//効果音として鳴らす
	static void PlaySE(const std::wstring& alias);

	//指定した音だけ止める
	static void Stop(const std::wstring& alias);

	//鳴ってる音全部止める
	static void StopAll();
};

