#include "Audio.h"
#include<windows.h>
#include<mmsystem.h>
#include<iostream>

#pragma comment(lib,"winmm.lib")

void Audio::Load(const std::wstring& filename, const std::wstring& alias)
{
	std::wstring cmd = L"open\"" + filename + L"\"alias" + alias;
	if (mciSendStringW(cmd.c_str(), nullptr, 0, nullptr) != 0)
	{
		std::cout << "エラー:音源の読み込み失敗\n";
	}
	else
	{
		std::cout << "ログ:音源[" << std::string(alias.begin(), alias.end()) << "]の読み込み成功\n";
	}
}

//BGM用
void Audio::PlayBGM(const std::wstring& alias)
{
	//ループ再生
	std::wstring cmdReset = L"seek" + alias + L"to start";
	mciSendStringW(cmdReset.c_str(), nullptr, 0, nullptr);


}