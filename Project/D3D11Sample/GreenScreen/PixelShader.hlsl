struct OutputVertex
{
	float4 pos : SV_POSITION;
	float4 color : OCOLOR;
};

// SHADER_VARS : register (b0)
//{
//	unsigned int txWidth;
//	unsigned int txHeight;
//	unsigned int txNumPixels;
//	unsigned int txNumLevels;
//	const unsigned int* txOffsets;
//	const unsigned int* txRaster;
//}

float4 main(OutputVertex inputPixel) : SV_Target
{
	return float4(0.5f, 0.23f, 0.67f, 1);
}