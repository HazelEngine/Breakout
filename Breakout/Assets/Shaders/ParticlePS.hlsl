// ParticlePS.hlsl

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

Texture2D ParticleTexture;
float4    Color;

float4 ParticlePS(PixelIn input) : SV_TARGET
{
	return Color * ParticleTexture.Sample(Sampler, input.TexCoords);
}