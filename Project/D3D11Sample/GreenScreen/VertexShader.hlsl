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
	float4 color : COLOR;
	float4 wPos : WPOSITION;
};

struct OutputVertex
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
	float4 wPos : WPOSITION0;
};

cbuffer matrixVars : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

OutputVertex main(InputVertex input)
{
    OutputVertex output = (OutputVertex) 0;
    output.pos = input.pos;
	output.uv = input.uv;
    output.normal = input.normal;
    output.color = input.color;
    //Do math here

	output.pos = mul(worldMatrix, output.pos);
    output.normal = mul(worldMatrix, output.normal);
	output.wPos = output.pos;


    output.pos = mul(viewMatrix, output.pos);
    output.pos = mul(projMatrix, output.pos);

    return output;
}