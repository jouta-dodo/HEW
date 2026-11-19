#pragma once
#include "Scene.h"
#include "Graphics.h" // ‰æ‘œ“Ç‚İ‚İE•`‰æ‚É•K—v

class TitleScene : public IScene
{
private:
    ComPtr<ID3D11ShaderResourceView> texBackground; // ”wŒi‰æ‘œ—p

public:
    void Init() override;
    void Update() override;
    void Draw() override;
};