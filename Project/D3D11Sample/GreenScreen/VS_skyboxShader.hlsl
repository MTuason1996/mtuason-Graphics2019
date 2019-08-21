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
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 worldView;
};

cbuffer timeVar : register(b1)
{
    float4 time;
}

OutputVertex main(InputVertex input)
{
    OutputVertex output = (OutputVertex) 0;
    output.pos = input.pos;
    output.uv = input.uv;
    output.normal = input.normal;
    //Do math here

    output.wPos = output.pos;
    output.pos = mul(worldView, output.pos);
    output.normal = mul(worldView, output.normal);



    output.pos = mul(viewMatrix, output.pos);
    output.pos = mul(projMatrix, output.pos);

    return output;
}