struct OutputVertex
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL0;
    float4 wPos : WPOSITION0;
};

TextureCube txDiffuse : register(t1);
SamplerState samLinear : register(s0);

float4 main(OutputVertex inputPixel) : SV_Target
{
    return txDiffuse.Sample(samLinear, (float3)(inputPixel.wPos));
    //return txDiffuse.Sample(samLinear, inputPixel.uv);
}