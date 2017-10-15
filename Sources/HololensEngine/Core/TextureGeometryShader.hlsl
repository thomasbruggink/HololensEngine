struct GSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	uint rtvId : TEXCOORD1;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	uint rtvId : SV_RenderTargetArrayIndex;
};

[maxvertexcount(3)]
void main(triangle GSInput input[3], inout TriangleStream<GSOutput> output)
{
	GSOutput element;
	[unroll(3)]
	for (uint i = 0; i < 3; i++)
	{
		element.pos = input[i].pos;
		element.tex = input[i].tex;
		element.rtvId = input[i].rtvId;
		element.normal = input[i].normal;
		output.Append(element);
	}
}