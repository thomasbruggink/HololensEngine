Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 baseColor = shaderTexture.Sample(SampleType, input.tex);

	float4 color = ambientColor;

	// Invert the light direction for calculations.
	float3 lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	float lightIntensity = saturate(dot(input.normal, lightDir));
	if (lightIntensity > 0.0f)
	{
		// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
		color += (diffuseColor * lightIntensity);
	}

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = saturate(color);
	color = color * baseColor;

	return color;
}
