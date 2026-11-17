#include "Player.h"
#include "Input.h"
#include <windows.h> // VK_ などの定数や GetAsyncKeyState に必要

Player::Player()
	: m_x(0.0f), m_y(0.0f)
	, m_width(100.0f), m_height(100.0f)
	, m_previousX(0.0f), m_previousY(0.0f)
	, m_velocityY(0.0f)
	, m_moveSpeed(10.0f)
	, m_jumpPower(12.0f)
	, m_gravity(0.5f)
	, m_isGrounded(false)
	, m_isDead(false)
	, m_spaceWasPressed(false)
{
}

Player::~Player()
{
}

void Player::Init(ComPtr<ID3D11ShaderResourceView> texture)
{
	m_texture = texture;
	m_x = 640.0f - 50.0f;
	m_width = 100.0f;
	m_height = 100.0f;
	m_y = 450.0f - m_height;

	m_velocityY = 0.0f;
	m_isGrounded = true;  // 最初から地面に立っている状態にする
	m_isDead = false;
	m_spaceWasPressed = false;
}

void Player::Update(float screenWidth, float screenHeight)
{
	m_previousX = m_x;
	m_previousY = m_y;

	if (m_isDead) return;

	// 左右移動（Aキー / Dキー、または矢印キー）
	if ((GetAsyncKeyState('A') & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000))
	{
		m_x -= m_moveSpeed;
	}
	if ((GetAsyncKeyState('D') & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000))
	{
		m_x += m_moveSpeed;
	}

	// 画面外制限
	if (m_x < 0.0f) m_x = 0.0f;
	if (m_x > screenWidth - m_width) m_x = screenWidth - m_width;

	// ジャンプ（スペースキー）
	bool spaceNow = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
	if (spaceNow && !m_spaceWasPressed && m_isGrounded)
	{
		m_velocityY = m_jumpPower;
		m_isGrounded = false;
	}
	m_spaceWasPressed = spaceNow;

	const float platformX1 = 100.0f;
	const float platformX2 = screenWidth - 100.0f;
	const float platformY = 450.0f;
	const bool overlapsPlatform = m_x + m_width >= platformX1 && m_x <= platformX2;

	if (m_isGrounded && !overlapsPlatform)
	{
		m_isGrounded = false;
	}

	if (!m_isGrounded)
	{
		const float previousFootY = m_previousY + m_height;

		m_velocityY -= m_gravity;
		m_y -= m_velocityY;

		const float footY = m_y + m_height;
		if (m_velocityY <= 0.0f && overlapsPlatform &&
			previousFootY <= platformY && footY >= platformY)
		{
			m_y = platformY - m_height;
			m_velocityY = 0.0f;
			m_isGrounded = true;
		}
		else if (m_y >= screenHeight - m_height)
		{
			m_y = screenHeight - m_height;
			m_velocityY = 0.0f;
			m_isGrounded = true;
		}
	}
}

void Player::Draw()
{
	if (m_texture.Get() != nullptr)
	{
		Graphics::DrawImage(m_x, m_y, m_width, m_height, m_texture, Color::White);
	}
}

void Player::Uninit()
{
	m_texture.Reset();
}