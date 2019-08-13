struct OutputVertex
{
	float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 main(OutputVertex inputPixel) : SV_Target
{
    return txDiffuse.Sample(samLinear, inputPixel.uv) /** inputPixel.color*/;
}