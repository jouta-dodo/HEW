#pragma once
#include "Graphics.h"

class Player
{
public:
	Player();
	~Player();

	void Init(ComPtr<ID3D11ShaderResourceView> texture);
	void Update(float screenWidth, float screenHeight);
	void UpdateGravity(float screenWidth, float screenHeight);
    void Draw(float cameraX = 0.0f);
	void Uninit();

	// ゲッター・セッター
	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	void SetPos(float x, float y) { m_x = x; m_y = y; }

	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }
	void SetSize(float w, float h) { m_width = w; m_height = h; }

	float GetVelocityY() const { return m_velocityY; }
	void SetVelocityY(float vy) { m_velocityY = vy; }

	bool IsGrounded() const { return m_isGrounded; }
	void SetGrounded(bool grounded) { m_isGrounded = grounded; }

	bool IsDead() const { return m_isDead; }
	void SetDead(bool dead) { m_isDead = dead; }

private:
	float m_x, m_y;          // 座標
	float m_width, m_height; // サイズ
	float m_previousX, m_previousY; // 前フレームの座標

	float m_velocityY;       // 縦方向の速度
	float m_moveSpeed;       // 移動速度
	float m_jumpPower;       // ジャンプ力
	float m_gravity;         // 重力

	bool m_isGrounded;       // 接地フラグ
	bool m_isDead;           // 死亡フラグ
	bool m_spaceWasPressed;  // スペースキー入力保持

	ComPtr<ID3D11ShaderResourceView> m_texture; // プレイヤー用テクスチャ
};