#pragma once
#include<windows.h>
#include<Xinput.h>
#include<vector>
#include<unordered_map>
#include<initializer_list>
#include<functional>
#pragma comment(lib,"xinput.lib")

enum class Key { Up, Down, Left,Right,Space,Enter,Escape,Shift,W,A,S,D,Z,X,C };
enum class Button{Up,Down,Left,Right,A,B,X,Y,Start,Back,L1,R1};


class Input
{
	private:
	static void BindActionImpl(int actionId, std::initializer_list<Key>keys, std::initializer_list<Button>buttons);
	static bool IsPressedImpl(int actionId);
	static bool IsHeldImpl(int actionId);
	static bool IsReleasedImpl(int actionId);

public:
	//=========================================
	//フレームワーク側毎フレーム一回だけ呼ぶ用
	//=========================================
	static void Update();

	//===================
	//バインドして使う用
	//===================
	template<typename T>
	static void BindAction(T action, std::initializer_list<Key>keys, std::initializer_list<Button>buttons)
	{
		BindActionImpl(static_cast<int>(action),keys,buttons);
	}

	//=================
	//コマンドの判定系
	//=================

	//押された瞬間
	template<typename T>
	static bool IsPressed(T action)
	{
		return IsPressedImpl(static_cast<int>(action));
	}

	//押しっぱなし
	template<typename T>
	static bool IsHeld(T action)
	{
		return IsHeldImpl(static_cast<int>(action));
	}

	//離したとき
	template<typename T>
	static bool IsReleased(T action)
	{
		return IsReleasedImpl(static_cast<int>(action));
	}

	//======================================
	// バインド無視で直接キーやボタン呼ぶ用
	//======================================

	static bool IsKeyPressed(Key key);
	static bool IsKeyHeld(Key key);
	static bool IsKeyReleased(Key key);

	static bool IsButtonPressed(Button btn);
	static bool IsButtonHeld(Button btn);
	static bool IsButtonRelesed(Button btn);


	Input() = delete;
};