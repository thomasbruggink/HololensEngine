Texture2D shaderTexture;
SamplerState SampleType;

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = shaderTexture.Sample(SampleType, input.tex);
	//float4 color = float4(0.5f, 0.5f, 0.5f, 0.5f);
	return color;
}
