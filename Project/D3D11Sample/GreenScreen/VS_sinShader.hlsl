//rule of three
//three things must match
// 1. C++ Vertex Struct
// 2. Input Layout
// 3. HLSL Vertex Struct

struct InputVertex
{
	float4 pos : POSITION;
	float2 uv : TEXTURE;
	float4 normal : NORMAL;
};

struct OutputVertex
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 normal : NORMAL0;
	float4 wPos : WPOSITION0;
};

cbuffer matrixVars : register(b0)
{
	float4x4 worldMatrix[6];
	float4x4 viewMatrix;
	float4x4 projMatrix;
    float4x4 worldView;
};

cbuffer timeVar : register(b1)
{
	float4 time;
}

OutputVertex main(InputVertex input, uint instance : SV_InstanceID)
{
	OutputVertex output = (OutputVertex)0;
	output.pos = input.pos;
	output.uv = input.uv;
	output.normal = input.normal;
	//Do math here

	output.pos = mul(worldMatrix[instance], output.pos);
	output.normal = mul(worldMatrix[instance], output.normal);

	output.pos.y += cos(output.pos.x * 0.1 + time.x);

	output.wPos = output.pos;



	output.pos = mul(viewMatrix, output.pos);
	output.pos = mul(projMatrix, output.pos);

	return output;
}