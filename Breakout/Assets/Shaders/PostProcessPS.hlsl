// PostProcessPS.hlsl

struct PixelIn
{
	float4 PosH      : SV_POSITION;
	float2 TexCoords : TEXCOORD;
};

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

Texture2D Scene;
float2    Offsets[9];
int       EdgeKernel[9];
float     BlurKernel[9];

bool Chaos;
bool Confuse;
bool Shake;

float4 PostProcessPS(PixelIn input) : SV_TARGET
{
	float3 color = float3(0.0f, 0.0f, 0.0f);
	float3 smp[9];

	// Sample from texture offsets if using convolution matrix
	if (Chaos || Shake)
	{
		for (int i = 0; i < 9; i++)
		{
			smp[i] = Scene.Sample(Sampler, input.TexCoords.xy + Offsets[i]).xyz;
		}
	}

	// Process effects
	if (Chaos)
	{
		for (int i = 0; i < 9; i++)
		{
			color += smp[i] * EdgeKernel[i];
		}
	}
	else if (Confuse)
	{
		color = 1.0f - Scene.Sample(Sampler, input.TexCoords).xyz;
	}
	else if (Shake)
	{
		for (int i = 0; i < 9; i++)
		{
			color += smp[i] * BlurKernel[i];
		}
	}
	else
	{
		color = Scene.Sample(Sampler, input.TexCoords).xyz;
	}

	return float4(color, 1.0f);
}