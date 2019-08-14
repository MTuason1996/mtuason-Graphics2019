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
};

struct OutputVertex
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
};

cbuffer matrixVars : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projMatrix;
};

cbuffer lightVars : register(b1)
{
    float4 dLight[3];
    float4 pLight[4];
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

    //Directional light
    float lightRDir = saturate(dot(dLight[1], output.normal) + 0.25f);
    float4 luminenceDir = lerp(float4(0, 0, 0, 1), dLight[2], lightRDir);

    //PointLight
    float4 pointSubSurf = pLight[0] - output.pos;
    float4 pLightFacing = normalize(pointSubSurf);
    float attenuation = 1.0f - saturate(length(pointSubSurf) / pLight[3].x);
    attenuation *= attenuation;

    float lightRPoint = (saturate(dot(pLightFacing, output.normal)) + 0.5f) * attenuation;
    float4 luminencePoint = lerp(float4(0, 0, 0, 1), pLight[2], lightRPoint);


    output.color = saturate(luminenceDir + luminencePoint);


	output.pos = mul(viewMatrix, output.pos);
	output.pos = mul(projMatrix, output.pos);


    return output;

}