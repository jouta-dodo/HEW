#pragma once
namespace Game
{
	//ゲーム機同時に一回だけ呼ばれるやつ
	void Init();

	//毎フレーム呼ばれるやつ
	void Update();

	//描画
	void Draw();
};