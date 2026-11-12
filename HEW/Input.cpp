#include "Input.h"
#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include<Xinput.h>
#include<vector>
#include<unordered_map>

#pragma comment(lib,"xinput.lib")

namespace
{
	//============================
	// 割り当てデータを持つ構造体
	//============================
	struct InputBinding
	{
		std::vector<Key>keys;
		std::vector<Button>buttons;
	};

	XINPUT_STATE s_padState = {};
	XINPUT_STATE s_prevPadState{};
	bool s_isPadConnected = false;

	bool s_currentRawKeys[256] = { false };
	bool s_prevRawKeys[256] = { false };

	std::unordered_map<int, InputBinding> s_bindings;
	std::unordered_map<int, bool> s_currentStates;
	std::unordered_map<int, bool> s_prevStates;

	//===========
	//キーコード
	//===========
	int ConvertKey(Key key)
	{
		switch (key)
		{
		case Key::Up:	return VK_UP;
		case Key::Down: return VK_DOWN;
		case Key::Left: return VK_LEFT;
		case Key::Right: return VK_RIGHT;
		case Key::Space: return VK_SPACE;
		case Key::Enter: return VK_RETURN;
		case Key::Escape: return VK_ESCAPE;
		case Key::Shift: return VK_SHIFT;
		case Key::W: return 'W';
		case Key::A: return 'A';
		case Key::S: return 'S';
		case Key::D: return 'D';
		case Key::Z: return 'Z';
		case Key::X: return 'X';
		case Key::C: return 'C';
		default:	 return 0;
		}
	}

	//=======
	//ボタン
	//=======
	WORD ConvertButton(Button btn)
	{
		switch (btn)
		{
		case Button::Up: return XINPUT_GAMEPAD_DPAD_UP;
		case Button::Down: return XINPUT_GAMEPAD_DPAD_DOWN;
		case Button::Left: return XINPUT_GAMEPAD_DPAD_LEFT;
		case Button::Right: return XINPUT_GAMEPAD_DPAD_RIGHT;
		case Button::A: return XINPUT_GAMEPAD_A;
		case Button::B: return XINPUT_GAMEPAD_B;
		case Button::X: return XINPUT_GAMEPAD_X;
		case Button::Y: return XINPUT_GAMEPAD_Y;
		case Button::Start: return XINPUT_GAMEPAD_START;
		case Button::Back: return XINPUT_GAMEPAD_BACK;
		case Button::L1: return XINPUT_GAMEPAD_LEFT_SHOULDER;
		case Button::R1: return XINPUT_GAMEPAD_RIGHT_SHOULDER;
		default: return 0;
		}
	}
}

void Input::Update()
{
	//コントローラーの状態更新
	s_prevPadState = s_padState;
	XINPUT_STATE newState = {};
	s_isPadConnected = (XInputGetState(0, &newState) == ERROR_SUCCESS);
	if (s_isPadConnected)
	{
		s_padState = newState;
	}
	else
	{
		ZeroMemory(&s_padState, sizeof(XINPUT_STATE));
	}

	//キーボードの状態更新
	for (int vk = 0;vk < 256;++vk)
	{
		s_prevRawKeys[vk] = s_currentRawKeys[vk];
		s_currentRawKeys[vk] = (GetAsyncKeyState(vk) & 0x8000) != 0;
	}

	//バインドされたアクションの状態更新
	for (auto& pair : s_bindings)
	{
		int actionId = pair.first;
		const InputBinding& binding = pair.second;

		s_prevStates[actionId] = s_currentStates[actionId];
		bool isPressed = false;

		for (Key k : binding.keys)
		{
			if (IsKeyHeld(k))
			{
				isPressed = true;
				break;
			}
		}

		if (!isPressed && s_isPadConnected)
		{
			for (Button b : binding.buttons)
			{
				if (IsButtonHeld(b))
				{
					isPressed = true;
					break;
				}
			}
		}

		s_currentStates[actionId] = isPressed;
	}
}

void Input::BindActionImpl(int actionId, std::initializer_list<Key>keys, std::initializer_list<Button>buttons)
{
	InputBinding binding;
	binding.keys = keys;
	binding.buttons = buttons;

	s_bindings[actionId] = binding;
	s_currentStates[actionId] = false;
	s_prevStates[actionId] = false;
}

bool Input::IsPressedImpl(int actionId)
{
	auto itCur = s_currentStates.find(actionId);
	auto itPrev = s_prevStates.find(actionId);
	if (itCur == s_currentStates.end() || itPrev == s_prevStates.end())return false;
	return itCur->second && !itPrev->second;
}

bool Input::IsHeldImpl(int actionId)
{
	auto itCur = s_currentStates.find(actionId);
	return (itCur != s_currentStates.end()) ? itCur->second : false;
}

bool Input::IsReleasedImpl(int actionId)
{
	auto itCur = s_currentStates.find(actionId);
	auto itPrev = s_prevStates.find(actionId);
	if (itCur == s_currentStates.end() || itPrev == s_prevStates.end())return false;
	return !itCur->second && itPrev->second;
}

bool Input::IsKeyPressed(Key key)
{
	int vk = ConvertKey(key);
	return (vk > 0 && vk < 256) ? (s_currentRawKeys[vk] && !s_prevRawKeys[vk]) : false;
}

bool Input::IsKeyHeld(Key key)
{
	int vk = ConvertKey(key);
	return (vk > 0 && vk < 256) ? s_currentRawKeys[vk] : false;
}

bool Input::IsKeyReleased(Key key)
{
	int vk = ConvertKey(key);
	return (vk > 0 && vk < 256) ? (!s_currentRawKeys[vk] && s_prevRawKeys[vk]) : false;
}

bool Input::IsButtonPressed(Button btn)
{
	if (!s_isPadConnected)return false;
	WORD mask = ConvertButton(btn);
	bool cur = (s_padState.Gamepad.wButtons & mask) != 0;
	bool prev = (s_prevPadState.Gamepad.wButtons & mask) != 0;
	return cur && !prev;
}

bool Input::IsButtonHeld(Button btn)
{
	if (!s_isPadConnected)return false;
	WORD mask = ConvertButton(btn);
	return (s_padState.Gamepad.wButtons & mask) != 0;
}

bool Input::IsButtonRelesed(Button btn)
{
	if (!s_isPadConnected)return false;
	WORD mask = ConvertButton(btn);
	bool cur = (s_padState.Gamepad.wButtons & mask) != 0;
	bool prev = (s_prevPadState.Gamepad.wButtons & mask) != 0;
	return !cur && prev;
}