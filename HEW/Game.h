#pragma once
namespace Game
{
	//ゲーム機同時に一回だけ呼ばれるやつ
	void Init();

	//毎フレーム呼ばれるやつ
	void Update();

	// ゴール接触などでステージクリアが確定した時に呼び出す
	void OnStageCleared();

	//描画
	void Draw();
};
//平岡作
/*
#pragma once
#include "Object.h"
#include "input.h"
class Game
{
private:
	Input input;
	Object m_player;
	Object m_ground;
	// 固定足場
	Object m_fixedPlatform;
	// 出現する5つのブロック
	Object m_redLeftPlatform;
	Object m_greenPlatform;
	Object m_yellowPlatform;
	Object m_redRightPlatform;
	Object m_purplePlatform;
	// 出現位置のガイド
	Object m_redLeftGuide;
	Object m_greenGuide;
	Object m_yellowGuide;
	Object m_redRightGuide;
	Object m_purpleGuide;
	float m_velocityY = 0.0f;
	float m_moveSpeed = 5.0f;
	float m_jumpPower = 9.0f;
	float m_gravity = 0.5f;
	bool m_isGrounded = true;
	bool m_spaceWasPressed = false;
	// ブロック切り替え
	int m_blockNumber = 1;
	int m_blockTimer = 0;
	bool m_isDead = false;
	// ガイドの縁（5か所 × 4辺）
	Object m_guideFrames[5][4];

	bool m_rWasPressed = false;
public:
	void Init(HWND hWnd);
	void Update(void);
	void Draw();
	void Uninit();
};
*/