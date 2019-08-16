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
    float4 sLight[4];
	float4 specular[2];
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

	// Specular lighting
	float4 viewDir = normalize(specular[1] - output.pos);

    //Directional light
    float lightRDir = saturate(dot(dLight[1], output.normal) + 0.25f);
    float4 luminenceDir = lerp(float4(0, 0, 0, 1), dLight[2], lightRDir);

	// spec directional
	float4 dLightHalfVec = normalize((-dLight[1]) + viewDir);
	float dLightIntensity = saturate(pow(dot(output.normal, dLightHalfVec), specular[0].y));
	luminenceDir = saturate(luminenceDir + (dLight[2] * specular[0].x * dLightIntensity));

    //PointLight
    float4 pointSubSurf = pLight[0] - output.pos;
    float4 pLightFacing = normalize(pointSubSurf);
    float pAtt = 1.0f - saturate(length(pointSubSurf) / pLight[3].x);
    pAtt *= pAtt;

    float lightRPoint = (saturate(dot(pLightFacing, output.normal)) + 0.75f) * pAtt;
    float4 luminencePoint = lerp(float4(0, 0, 0, 1), pLight[2], lightRPoint);

    // Spot Light
    float4 sLightFacing = normalize(sLight[0] - output.pos);
    float surfaceRatio = saturate(dot(-sLightFacing, sLight[1]));

    float sAtt = 1.0 - saturate((sLight[3].x - surfaceRatio) / (sLight[3].x - sLight[3].y));
    sAtt *= sAtt;
    
    float lightRSpot = (saturate(dot(sLightFacing, output.normal)) + 1.0f) * sAtt;
    float4 luminenceSpot = lerp(float4(0, 0, 0, 1), sLight[2], lightRSpot);

	// specular Spot
	float4 sLightHalfVec = normalize((-sLight[1]) + viewDir);
	float sLightIntensity = saturate(pow(dot(output.normal, dLightHalfVec), specular[0].y));
	luminenceSpot = saturate(luminenceSpot + (sLight[2] * specular[0].x * sLightIntensity));


    output.color = saturate(luminenceDir + luminencePoint + luminenceSpot);


	output.pos = mul(viewMatrix, output.pos);
	output.pos = mul(projMatrix, output.pos);


    return output;

}