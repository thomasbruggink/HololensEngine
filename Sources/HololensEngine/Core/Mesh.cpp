#include "Mesh.h"
#include "Engine.h"
#include "HoloWindow.h"
#include "HoloDrawEngine.h"
#include "ColorShader.h"

using namespace HololensEngineCore;
using namespace CoreTypes;
using namespace DirectX;
using namespace Platform;

Mesh::Mesh(Vector3^ location, ObjReader^ reader, HoloDrawEngine^ drawEngine)
{
	Init(location, reader, drawEngine, Color::White());
}

Mesh::Mesh(Vector3^ location, ObjReader^ reader, HoloDrawEngine^ drawEngine, Color^ color)
{
	Init(location, reader, drawEngine, color);
}

void Mesh::Init(Vector3^ location, ObjReader^ reader, HoloDrawEngine^ drawEngine, Color^ color)
{
	_location = location;
	_device = drawEngine->GetDevice();
	_context = drawEngine->GetContext();
	_size = reader->Size;
	_rotationPoint = RotationPoint::Center;
	_rotation = ref new Vector3(0.0f, 0.0f, 0.0f);
	_lightDirection = ref new Vector3(0.0f, 0.0f, 0.0f);
	_scale = ref new Vector3(1.0f, 1.0f, 1.0f);
	_ambientLight = 1.0f;

	_vertexCount = reader->VertexCount;
	_vertice = new Vertex[reader->VertexCount];
	_indexCount = reader->IndexCount;
	_indices = new unsigned long[reader->IndexCount];

	for (unsigned int i = 0; i < _vertexCount; i++)
	{
		VertexTextureNormal^ pair = reader->Vertice->GetAt(i);
		_vertice[i].Pos.x = pair->Vertex->X;
		_vertice[i].Pos.y = pair->Vertex->Y;
		_vertice[i].Pos.z = pair->Vertex->Z;

		_vertice[i].Color.x = color->R;
		_vertice[i].Color.y = color->G;
		_vertice[i].Color.z = color->B;
		_vertice[i].Color.w = color->A;

		_vertice[i].Norm.x = pair->Normal->X;
		_vertice[i].Norm.y = pair->Normal->Y;
		_vertice[i].Norm.z = pair->Normal->Z;
	}

	for (unsigned int i = 0; i < _indexCount; i++)
	{
		_indices[i] = (unsigned long)reader->Indices->GetAt(i);
	}

	_colorShader = ref new ColorShader(drawEngine);
	_colorShader->Initialize();
}

void Mesh::Draw()
{
	if (_vertexBuffer == NULL)
	{
		//Normalize texture coordinates to 0.0 - 1.0
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * _vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = _vertice;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		result = _device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);

		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		indexData.pSysMem = _indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = _device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
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
	world *= XMMatrixScaling(_scale->X, _scale->Y, _scale->Z);
	world *= XMMatrixTranslation(_location->X, _location->Y, _location->Z);

	//Draw mesh
	//_colorShader->Render(_indexCount, world, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f));
	_colorShader->Render(_indexCount, world, DirectX::XMFLOAT3(_lightDirection->X, _lightDirection->Y, _lightDirection->Z), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(_ambientLight, _ambientLight, _ambientLight, 1.0f));
}

void Mesh::Move(Vector3^ move)
{
	_location = _location->Plus(move);
}

void Mesh::MoveTo(Vector3^ location)
{
	_location = location;
}

void Mesh::ChangeRotationPoint(RotationPoint rotationPoint)
{
	_rotationPoint = rotationPoint;
}

CoreTypes::Vector3^ Mesh::Location()
{
	return _location;
}

CoreTypes::Vector3^ Mesh::Rotation()
{
	return _rotation;
}

CoreTypes::Vector3^ Mesh::LightDirection()
{
	return _lightDirection;
}

CoreTypes::Vector3^ Mesh::Scale()
{
	return _scale;
}

float Mesh::AmbientLight()
{
	return _ambientLight;
}

void Mesh::AmbientLight(float input)
{
	_ambientLight = input;
}

Mesh::~Mesh()
{

}
