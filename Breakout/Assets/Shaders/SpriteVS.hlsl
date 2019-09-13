// SpriteVS.hlsl

struct VertexIn
{
	float4 Vertex : POSITION; // float2 Position, float2 TexCoords
};

struct VertexOut
{
	float4 PosW      : SV_POSITION;
	float2 TexCoords : TEXCOORD;
};

float4x4 Model;
float4x4 ViewProjection;

VertexOut SpriteVS(VertexIn input)
{
	VertexOut output;
	output.PosW = mul(mul(float4(input.Vertex.xy, 0.0f, 1.0f), Model), ViewProjection);
	output.TexCoords = input.Vertex.zw;

	return output;
}