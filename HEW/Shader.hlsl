Texture2D tex : register(t0);
SamplerState samp : register(s0);

//頂点シェーダーでつかうデータ
struct VS_INPUT
{
    float3 pos : POSITION;
    float4 col : COLOR;
    float2 uv : TEXCOORD;
};

//ピクセルシェーダーで使用するデータ
struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 uv : TEXCOORD;
};

// 頂点（カド）の処理
PS_IN VS_Main(VS_INPUT input)
{
    PS_IN output;
    output.pos = float4(input.pos, 1.0f);
    output.col = input.col;
    output.uv = input.uv;
    return output;
}

// 色塗りの処理
float4 PS_Main(PS_IN input) : SV_Target
{
    float4 texColor = tex.Sample(samp, input.uv);
    return texColor*input.col;
}