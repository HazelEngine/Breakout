// SpritePS.hlsl

struct PixelIn
{
	float4 PosW      : SV_POSITION;
	float2 TexCoords : TEXCOORD;
};

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D SpriteTexture;
float3 SpriteColor;

float4 SpritePS(PixelIn input) : SV_TARGET
{
	return float4(SpriteColor, 1.0f) * SpriteTexture.Sample(Sampler, input.TexCoords);
}