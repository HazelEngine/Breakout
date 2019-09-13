// PostProcessVS.hlsl

struct VertexIn
{
	float4 Vertex : POSITION; // float2 Position, float2 TexCoords
};

struct VertexOut
{
	float4 PosH      : SV_POSITION;
	float2 TexCoords : TEXCOORD;
};

bool  Chaos;
bool  Confuse;
bool  Shake;
float Time;

VertexOut PostProcessVS(VertexIn input)
{
	float4 posH = float4(input.Vertex.xy, 0.0f, 1.0f);
	float2 tex = input.Vertex.zw;

	if (Chaos)
	{
		float strength = 0.3f;
		float2 pos = float2(tex.x + sin(Time) * strength, tex.y + cos(Time) * strength);
		tex = pos;
	}
	else if (Confuse)
	{
		tex = float2(1.0f - tex.x, 1.0f - tex.y);
	}

	if (Shake)
	{
		float strength = 0.01f;
		posH.x += cos(Time * 10.0f) * strength;
		posH.y += cos(Time * 15.0f) * strength;
	}

	VertexOut output;
	output.PosH = posH;
	output.TexCoords = tex;

	return output;
}