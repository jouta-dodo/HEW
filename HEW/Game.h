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