#pragma once
#include "Scene.h"
#include "Graphics.h"

class TitleScene : public IScene
{
private:
    ComPtr<ID3D11ShaderResourceView> texBackground;
    bool spaceWasPressed = false;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
};