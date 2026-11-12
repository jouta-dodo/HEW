#pragma once
#include<memory>

//===============================
//シーン管理にこれを継承して使う
//===============================
class IScene
{
public:
	virtual ~IScene() = default;

	//初期化(開始時一回)
	virtual void Init() = 0;

	//更新処理(毎フレーム)
	virtual void Update() = 0;

	//描画(毎フレーム)
	virtual void Draw() = 0;
};


//=====================
//シーン切り替えるやつ
//=====================
class SceneManager
{
public:
	//シーン切り替え
	static void ChangeScene(IScene* nextScene);

	static void Update();
	static void Draw();
private:
	static std::unique_ptr<IScene>s_currentScene;

	SceneManager() = delete;
};
