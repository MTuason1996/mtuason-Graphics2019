struct OutputVertex
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 normal : NORMAL0;
	float4 color : COLOR0;
	float4 wPos : WPOSITION0;
};

cbuffer lightVars : register(b0)
{
	float4 dLight[3];
	float4 pLight[4];
	float4 sLight[4];
	float4 specular[2];
};

cbuffer timeVar : register(b1)
{
	float4 time;
}



Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 main(OutputVertex inputPixel) : SV_Target
{
	float amp = 0.5f;
	float freq = 0.5f;
	float wave = amp * cos(freq * (inputPixel.wPos.x * 0.5f + time.x));

	// Specular lighting
	float4 viewDir = normalize(specular[1] - inputPixel.wPos);

	//Directional light
	float lightRDir = saturate(dot(dLight[1], inputPixel.normal) + 0.25f);
	float4 luminenceDir = lerp(float4(0, 0, 0, 1), dLight[2], lightRDir);

	//simulate clouds
	if (length(dLight[2]) > 0)
		luminenceDir = lerp(luminenceDir, float4(0,0,0, 1), wave);

	// spec directional
	float4 dLightHalfVec = normalize((-dLight[1]) + viewDir);
	float dLightIntensity = saturate(pow(dot(inputPixel.normal, dLightHalfVec), specular[0].y));
	luminenceDir = saturate(luminenceDir + (dLight[2] * specular[0].x * dLightIntensity));

	//PointLight
	float4 pointSubSurf = pLight[0] - inputPixel.wPos;
	float4 pLightFacing = normalize(pointSubSurf);
	float pAtt = 1.0f - saturate(length(pointSubSurf) / pLight[3].x);
	pAtt *= pAtt;

	float lightRPoint = (saturate(dot(pLightFacing, inputPixel.normal)) + 0.75f) * pAtt;
	float4 luminencePoint = lerp(float4(0, 0, 0, 1), pLight[2], lightRPoint);

	// Spot Light
	float4 sLightFacing = normalize(sLight[0] - inputPixel.wPos);
	float surfaceRatio = saturate(dot(-sLightFacing, sLight[1]));

	float sAtt = 1.0 - saturate((sLight[3].x - surfaceRatio) / (sLight[3].x - sLight[3].y));
	sAtt *= sAtt;

	float lightRSpot = (saturate(dot(sLightFacing, inputPixel.normal)) + 0.75f) * sAtt;
	float4 luminenceSpot = lerp(float4(0, 0, 0, 1), sLight[2], lightRSpot);

	// specular Spot
	float4 sLightHalfVec = normalize((-sLight[1]) + viewDir);
	float sLightIntensity = saturate(pow(dot(inputPixel.normal, dLightHalfVec), specular[0].y));
	luminenceSpot = saturate(luminenceSpot + (sLight[2] * specular[0].x * sLightIntensity));

	inputPixel.color = saturate(luminenceDir + luminencePoint + luminenceSpot);

	return txDiffuse.Sample(samLinear, inputPixel.uv) * inputPixel.color;
}