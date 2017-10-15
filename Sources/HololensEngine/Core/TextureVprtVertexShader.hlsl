cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
};

// A constant buffer that stores each set of view and projection matrices in column-major format.
cbuffer ViewProjectionConstantBuffer : register(b1)
{
	float4x4 viewProjection[2];
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	uint instId : SV_InstanceID;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	uint rtvId : SV_RenderTargetArrayIndex;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	int idx = input.instId % 2;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewProjection[idx]);
	output.rtvId = idx;

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// Normalize the normal vector.
	output.normal = normalize(output.normal);

	return output;
}