#include "Graphics.h"
#include<d3dcompiler.h>
#include<wincodec.h>
#include<vector>
#include<iostream>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"windowscodecs.lib")

//static変数の実体
ComPtr<ID3D11Device> Graphics::s_device;
ComPtr<ID3D11DeviceContext> Graphics::s_context;
ComPtr<IDXGISwapChain> Graphics::s_swapChain;
ComPtr<ID3D11RenderTargetView> Graphics::s_renderTargetView;
ComPtr<ID3D11Buffer>Graphics::s_vertexBuffer;
ComPtr<ID3D11VertexShader>Graphics::s_vertexShader;
ComPtr<ID3D11PixelShader>Graphics::s_pixelShader;
ComPtr<ID3D11InputLayout>Graphics::s_inputLayout;

ComPtr<ID3D11SamplerState> Graphics::s_samplerState;
ComPtr<ID3D11ShaderResourceView> Graphics::s_whiteTexture;

int Graphics::s_screenWidth = 0;
int Graphics::s_screenHeight = 0;

bool Graphics::Init(HWND hwnd, int width, int height)
{
	CoInitialize(nullptr);

	s_screenWidth = width;
	s_screenHeight = height;

	if (!hwnd)
	{
		std::cout << "ウィンドウ作成失敗";
		return false;
	}

	//スワップチェーン
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featurLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featurLevel, 1, D3D11_SDK_VERSION, &sd, &s_swapChain, &s_device, nullptr, &s_context);
	if (FAILED(hr))
	{
		std::cout << "DirectX初期化失敗";
		return false;
	}

	//レンダーターゲットビュー
	ComPtr<ID3D11Texture2D>backBuffer;
	hr = s_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
	{
		std::cout << "バックバッファー取得エラー";
		return false;
	}
	hr = s_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &s_renderTargetView);
	if (FAILED(hr))
	{
		std::cout << "レンダーターゲット作成エラー";
		return false;
	}

	s_context->OMSetRenderTargets(1, s_renderTargetView.GetAddressOf(), nullptr);

	//ビューポート
	D3D11_VIEWPORT vp = {};
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	s_context->RSSetViewports(1, &vp);

	ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;

	// 頂点シェーダーの読み込み
	hr = D3DCompileFromFile(L"Shader.hlsl", nullptr, nullptr, "VS_Main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		std::cout << "頂点シェーダーの読み込みエラー";	// エラー内容をVSの出力に出す！
		return false;
	}

	// ピクセルシェーダーの読み込み
	hr = D3DCompileFromFile(L"C:\\Users\\PC_User\\Desktop\\AT42A\\HEW\\HEW\\Shader.hlsl", nullptr, nullptr, "PS_Main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
	if (FAILED(hr)) 
	{
		if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		std::cout << "ピクセルシェーダーの読み込みエラー";
	}

	s_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &s_vertexShader);
	s_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &s_pixelShader);

	// 入力レイアウトの作成
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,28,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	s_device->CreateInputLayout(layout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &s_inputLayout);

	// 頂点バッファの作成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	s_device->CreateBuffer(&bd, nullptr, &s_vertexBuffer);
	if (FAILED(hr))
	{
		std::cout << "頂点バッファの作成エラー";
		return false;
	}

	//画像の拡大縮小の管理
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	s_device->CreateSamplerState(&sampDesc, &s_samplerState);

	D3D11_TEXTURE2D_DESC td = {};
	td.Width = 1;
	td.Height = 1;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	unsigned int whitePixel = 0xFFFFFFFF;
	D3D11_SUBRESOURCE_DATA sdData = {};
	sdData.pSysMem = &whitePixel;
	sdData.SysMemPitch = 4;
	ComPtr<ID3D11Texture2D>whiteTex;
	s_device->CreateTexture2D(&td, &sdData, &whiteTex);
	s_device->CreateShaderResourceView(whiteTex.Get(), nullptr, &s_whiteTexture);

	return true;
}

void Graphics::Uninit()
{
	s_whiteTexture.Reset();
	s_samplerState.Reset();
	s_vertexBuffer.Reset();
	s_inputLayout.Reset();
	s_vertexShader.Reset();
	s_pixelShader.Reset();
	s_renderTargetView.Reset();
	s_swapChain.Reset();
	s_context.Reset();
	s_device.Reset();

	CoUninitialize(); //片付け
}

ComPtr<ID3D11ShaderResourceView> Graphics::LoadTexture(const wchar_t* filename)
{
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<IWICImagingFactory> wicFactory;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)))) return nullptr;

	ComPtr<IWICBitmapDecoder> decoder;
	if (FAILED(wicFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder))) return nullptr;

	ComPtr<IWICBitmapFrameDecode> frame;
	if (FAILED(decoder->GetFrame(0, &frame))) return nullptr;

	ComPtr<IWICFormatConverter> converter;
	if (FAILED(wicFactory->CreateFormatConverter(&converter))) return nullptr;
	if (FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))) return nullptr;

	UINT width, height;
	converter->GetSize(&width, &height);
	std::vector<BYTE>pixels(width * height * 4);
	if (FAILED(converter->CopyPixels(nullptr, width * 4, pixels.size(), pixels.data()))) return nullptr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = pixels.data();
	initData.SysMemPitch = width * 4;

	ComPtr<ID3D11Texture2D> texture;
	if (FAILED(s_device->CreateTexture2D(&desc, &initData, &texture))) return nullptr;
	if(FAILED(s_device->CreateShaderResourceView(texture.Get(),nullptr,&srv))) return nullptr;

	return srv;
}

void Graphics::BeginDraw(const ColorF& clearColor)
{
	if (!s_context)return;
	float color[4] = { clearColor.r,clearColor.g,clearColor.b,clearColor.a };
	s_context->ClearRenderTargetView(s_renderTargetView.Get(), color);
}

void Graphics::EndDraw()
{
	if (!s_swapChain)return;
	s_swapChain->Present(1, 0);
}

void Graphics::DrawRect(float x, float y, float w, float h, const ColorF& color)
{
	DrawImage(x, y, w, h, s_whiteTexture, color);
}

void Graphics::DrawImage(float x, float y, float w, float h, ComPtr<ID3D11ShaderResourceView> texture, const ColorF& color)
{
	if (!s_context || !s_vertexBuffer||!texture)return;

	float fWidth = (float)s_screenWidth;
	float fHeight = (float)s_screenHeight;

	float left = (x / s_screenWidth) * 2.0f - 1.0f;
	float right = ((x + w) / s_screenWidth) * 2.0f - 1.0f;
	float top = -((y / s_screenHeight) * 2.0f - 1.0f);
	float bottom = -(((y + h) / s_screenHeight) * 2.0f - 1.0f);

	Vertex vertices[4] =
	{
		{left,top,0.0f,color.r,color.g,color.b,color.a,0.0f,0.0f},
		{right,top,0.0f,color.r,color.g,color.b,color.a,1.0f,0.0f},
		{left,bottom,0.0f,color.r,color.g,color.b,color.a,0.0f,1.0f},
		{right,bottom,0.0f,color.r,color.g,color.b,color.a,1.0f,1.0f}
	};

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	s_context->Map(s_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, vertices, sizeof(vertices));
	s_context->Unmap(s_vertexBuffer.Get(), 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	s_context->IASetInputLayout(s_inputLayout.Get());
	s_context->IASetVertexBuffers(0, 1, s_vertexBuffer.GetAddressOf(), &stride, &offset);
	s_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	s_context->VSSetShader(s_vertexShader.Get(), nullptr, 0);
	s_context->PSSetShader(s_pixelShader.Get(), nullptr, 0);

	s_context->PSSetShaderResources(0, 1, texture.GetAddressOf());
	s_context->PSSetSamplers(0, 1, s_samplerState.GetAddressOf());
	s_context->Draw(4, 0);
}