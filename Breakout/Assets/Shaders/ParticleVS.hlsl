// ParticleVS.hlsl

struct VertexIn
{
	float4 Vertex : POSITION; // float2 Position, float2 TexCoords
};

struct VertexOut
{
	float4 PosW      : SV_POSITION;
	float2 TexCoords : TEXCOORD;
};

float4x4 ViewProjection;
float2   Offset;
float    Scale;

VertexOut ParticleVS(VertexIn input)
{
	VertexOut output;
	output.PosW = mul(float4(input.Vertex.xy * Scale + Offset, 0.0f, 1.0f), ViewProjection);
	output.TexCoords = input.Vertex.zw;

	return output;
}