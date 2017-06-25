struct GSInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	uint rtvId : TEXCOORD1;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
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
		element.color = input[i].color;
		element.rtvId = input[i].rtvId;
		output.Append(element);
	}
}