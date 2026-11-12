#pragma once
#include<windows.h>
#include<d3d11.h>
#include<wrl/client.h>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

//===============
//色を扱う構造体
//===============
struct ColorF
{
	float r, g, b, a;

	//本来のやり方
	constexpr ColorF(float _r, float _g, float _b, float _a = 1.0f) :r(_r), g(_g), b(_b), a(_a){}

	//絵描きソフトと同じ感じのやつ
	constexpr ColorF(int _r,int _g,int _b,int _a=255):r(_r/255.0f),g(_g/255.0f), b(_b / 255.0f), a(_a / 255.0f) {}
};

//=============================
//よく使う色の名前のプリセット
//=============================
namespace Color
{
	constexpr ColorF White(1.0f, 1.0f, 1.0f);
	constexpr ColorF Black(0.0f, 0.0f, 0.0f);
	constexpr ColorF Red(1.0f, 0.0f, 0.0f);
	constexpr ColorF Green(0.0f, 1.0f, 0.0f);
	constexpr ColorF Blue(0.0f, 0.0f, 1.0f);
	constexpr ColorF Yellow(1.0f, 1.0f, 0.0f);
	constexpr ColorF Cyan(0.0f, 1.0f, 1.0f);
	constexpr ColorF Magenta(1.0f, 0.0f, 1.0f);
	
	constexpr ColorF Orange(255, 165, 0);
	constexpr ColorF Purple(128, 0, 128);
	constexpr ColorF SkyBlue(135, 206, 235);
	constexpr ColorF Gray(128, 128, 128);
}

struct Vertex
{
	float x, y, z;
	float r, g, b, a;
	float u, v;
};

class Graphics
{
public:
	//初期化
	static bool Init(HWND hwnd, int width, int height);
	static void Uninit();

	//描画
	static void BeginDraw(const ColorF& clearColor = Color::Black);

	//描画終了
	static void EndDraw();

	static void DrawRect(float x, float y, float w, float h, const ColorF& color);

	//画像貼り付けのやつ
	static ComPtr<ID3D11ShaderResourceView> LoadTexture(const wchar_t* filename);
	static void DrawImage(float x, float y, float w, float h, ComPtr<ID3D11ShaderResourceView> texture, const ColorF& color = Color::White);

private:
	static ComPtr<ID3D11Device> s_device;
	static ComPtr<ID3D11DeviceContext> s_context;
	static ComPtr<IDXGISwapChain> s_swapChain;
	static ComPtr<ID3D11RenderTargetView> s_renderTargetView;

	static ComPtr<ID3D11Buffer>s_vertexBuffer;
	static ComPtr<ID3D11VertexShader>s_vertexShader;
	static ComPtr<ID3D11PixelShader>s_pixelShader;
	static ComPtr<ID3D11InputLayout>s_inputLayout;

	static ComPtr<ID3D11SamplerState> s_samplerState;
	static ComPtr<ID3D11ShaderResourceView> s_whiteTexture;

	static int s_screenWidth;
	static int s_screenHeight;

	Graphics() = delete;
};