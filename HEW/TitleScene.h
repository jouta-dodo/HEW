#pragma once
#include "Scene.h"
#include "Graphics.h"

class TitleScene : public IScene
{
private:
    ComPtr<ID3D11ShaderResourceView> texLogo;
    ComPtr<ID3D11ShaderResourceView> texStart;
    ComPtr<ID3D11ShaderResourceView> texContinue;
    ComPtr<ID3D11ShaderResourceView> texEnd;
    int selectedOption = 0;
    bool spaceWasPressed = false;
    bool upWasPressed = false;
    bool downWasPressed = false;
    bool enterWasPressed = false;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
};