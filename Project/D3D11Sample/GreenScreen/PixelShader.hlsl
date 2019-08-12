struct OutputVertex
{
	float4 pos : SV_POSITION;
	float4 color : OCOLOR;
};

float4 main(OutputVertex inputPixel) : SV_Target
{
    return inputPixel.color;
}