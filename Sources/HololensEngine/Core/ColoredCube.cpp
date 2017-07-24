#include "ColoredCube.h"
#include "Engine.h"
#include "HoloWindow.h"
#include "HoloDrawEngine.h"
#include "ColorShader.h"

using namespace HololensEngineCore;
using namespace CoreTypes;
using namespace DirectX;
using namespace Platform;

ColoredCube::ColoredCube(Vector3^ location, CubeSize^ size, HoloDrawEngine^ drawEngine)
{
	Construct(location, size, Color::Transparent(), drawEngine);
}

ColoredCube::ColoredCube(Vector3^ location, CubeSize^ size, Color^ color, HoloDrawEngine^ drawEngine)
{
	Construct(location, size, color, drawEngine);
}

void ColoredCube::Construct(Vector3^ location, CubeSize^ size, Color^ color, HoloDrawEngine^ drawEngine)
{
	_location = location;
	_size = size;
	_device = drawEngine->GetDevice();
	_context = drawEngine->GetContext();
	_rotationPoint = RotationPoint::Center;
	_rotation = ref new Vector3(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < 8; i++)
	{
		_color.push_back(color);
	}

	_colorShader = ref new ColorShader(drawEngine);
	_colorShader->Initialize();
}

void ColoredCube::Draw()
{
	if (_vertexBuffer == NULL)
	{
		//Normalize texture coordinates to 0.0 - 1.0
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;

		float left = 0;
		float right = _size->Width;
		float top = 0;
		float bottom = _size->Height;
		float back = 0;
		float front = _size->Depth;

		std::array<Vertex, 8> cubeVertices =
		{ {
				/*0*/{ XMFLOAT3(right, bottom, front),	XMFLOAT4{ (float)_color[0]->R, (float)_color[0]->G, (float)_color[0]->B, (float)_color[0]->A } }, //Right bottom front
				/*1*/{ XMFLOAT3(right, bottom,  back),	XMFLOAT4{ (float)_color[1]->R, (float)_color[1]->G, (float)_color[1]->B, (float)_color[1]->A } }, //Right bottom rear
				/*2*/{ XMFLOAT3(right,  top, front),	XMFLOAT4{ (float)_color[2]->R, (float)_color[2]->G, (float)_color[2]->B, (float)_color[2]->A } }, //Right top front
				/*3*/{ XMFLOAT3(right,  top, back),		XMFLOAT4{ (float)_color[3]->R, (float)_color[3]->G, (float)_color[3]->B, (float)_color[3]->A } }, //Right top rear
				/*4*/{ XMFLOAT3(left, bottom, front),	XMFLOAT4{ (float)_color[4]->R, (float)_color[4]->G, (float)_color[4]->B, (float)_color[4]->A } }, //Left bottom front
				/*5*/{ XMFLOAT3(left, bottom,  back),	XMFLOAT4{ (float)_color[5]->R, (float)_color[5]->G, (float)_color[5]->B, (float)_color[5]->A } }, //Left bottom rear
				/*6*/{ XMFLOAT3(left,  top, front),		XMFLOAT4{ (float)_color[6]->R, (float)_color[6]->G, (float)_color[6]->B, (float)_color[6]->A } }, //Left top front
				/*7*/{ XMFLOAT3(left,  top,  back),		XMFLOAT4{ (float)_color[7]->R, (float)_color[7]->G, (float)_color[7]->B, (float)_color[7]->A }	}, //Left top rear
			} };

		std::array<unsigned short, 36> cubeIndices =
		{ {
				1,0,2, // +x
				1,2,3,

				4,5,7, // -x
				4,7,6,

				1,5,4, // -y
				4,0,1,

				2,6,7, // +y
				2,7,3,

				0,4,6, // +z
				0,6,2,

				5,1,3, // -z
				5,3,7,
			} };

		Vertex* vertices = new Vertex[cubeVertices.size()];
		unsigned long * indices = new unsigned long[36];

		for (unsigned int i = 0; i < 8; i++)
		{
			vertices[i] = cubeVertices[i];
		}

		for (unsigned int i = 0; i < 36; i++)
		{
			indices[i] = cubeIndices[i];
		}

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * cubeVertices.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		int indexBuffer = 0;

		result = _device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);

		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * 36;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = _device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);

		delete[] vertices;
		delete[] indices;
	}

	//Draw
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	_context->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	_context->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, offset);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world = XMMatrixIdentity();
	if (_rotationPoint == RotationPoint::Center)
		world *= XMMatrixTranslation(-_size->Width / 2.0f, -_size->Height / 2.0f, -_size->Depth / 2.0f);
	world *= XMMatrixRotationRollPitchYawFromVector({ XMConvertToRadians(_rotation->X), XMConvertToRadians(_rotation->Y), XMConvertToRadians(_rotation->Z) });
	world *= XMMatrixTranslation(_location->X, _location->Y, _location->Z);

	//Draw texture
	_colorShader->Render(36, world);
}

void ColoredCube::Move(Vector3^ move)
{
	_location = _location->Plus(move);
}

void ColoredCube::MoveTo(Vector3^ location)
{
	_location = location;
}

void ColoredCube::ChangeRotationPoint(RotationPoint rotationPoint)
{
	_rotationPoint = rotationPoint;
}

void ColoredCube::ChangeColor(Color^ color)
{
	for (unsigned int i = 0; i < 8; i++)
	{
		_color[i] = color;
	}
}

void ColoredCube::ChangeColor(int position, Color^ color)
{
	_color[position] = color;
}

CoreTypes::Vector3^ ColoredCube::Location()
{
	return _location;
}

CoreTypes::Vector3^ ColoredCube::Rotation()
{
	return _rotation;
}

ColoredCube::~ColoredCube()
{
	
}
