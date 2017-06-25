#include "TexturedCube.h"
#include "Engine.h"
#include "HoloWindow.h"
#include "HoloDrawEngine.h"
#include "TextureShader.h"

using namespace HololensEngineCore;
using namespace CoreTypes;
using namespace DirectX;
using namespace Platform;

TexturedCube::TexturedCube(CoreTypes::Vector3^ location, CoreTypes::CubeSize^ size, HoloDrawEngine^ drawEngine, CoreTypes::ImageData^ imageData)
{
	_location = location;
	_size = size;
	_device = drawEngine->GetDevice();
	_context = drawEngine->GetContext();
	_rotationPoint = RotationPoint::Center;
	_rotation = ref new Vector3(0.0f, 0.0f, 0.0f);

	_textureShader = ref new TextureShader(drawEngine);
	_textureShader->Initialize();
	ConvertImageData(imageData);
}

void TexturedCube::ConvertImageData(ImageData^ imageData)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = imageData->Width;
	textureDesc.Height = imageData->Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	switch (imageData->Format)
	{
	case ImageDataFormat::R8G8B8:
		textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM;
		break;
	case ImageDataFormat::R8G8B8A8:
		textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA resourceData;
	_imageData = new unsigned char[imageData->Data->Length];
	memcpy(_imageData, imageData->Data->begin(), imageData->Data->Length);

	resourceData.pSysMem = _imageData;
	resourceData.SysMemPitch = imageData->Width * (int)imageData->Format;
	resourceData.SysMemSlicePitch = 0;

	HRESULT textResult = _device->CreateTexture2D(&textureDesc, &resourceData, &_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
	shaderDesc.Format = textureDesc.Format;
	shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderDesc.Texture2D.MostDetailedMip = 0;
	shaderDesc.Texture2D.MipLevels = -1;

	HRESULT result = _device->CreateShaderResourceView(_texture, &shaderDesc, &_shaderResourceView);
	_context->GenerateMips(_shaderResourceView);
}

void TexturedCube::Draw()
{
	if (_vertexBuffer == NULL)
	{
		//Normalize texture coordinates to 0.0 - 1.0
		float ttop = 0;
		float tbottom = 1;
		float tleft = 0;
		float tright = 1;

		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;

		float left = 0;
		float right = _size->Width;
		float top = 0;
		float bottom = _size->Height;
		float back = 0;
		float front = _size->Depth;

		std::array<TexVertex, 8> cubeVertices =
		{ {
				/*0*/{ XMFLOAT3(right, bottom, front),	XMFLOAT2(tright, tbottom) }, //Right bottom front
				/*1*/{ XMFLOAT3(right, bottom,  back),	XMFLOAT2(tright, tbottom) }, //Right bottom rear
				/*2*/{ XMFLOAT3(right,  top, front),	XMFLOAT2(tright, ttop) }, //Right top front
				/*3*/{ XMFLOAT3(right,  top, back),		XMFLOAT2(tright, ttop) }, //Right top rear
				/*4*/{ XMFLOAT3(left, bottom, front),	XMFLOAT2(tleft, tbottom) }, //Left bottom front
				/*5*/{ XMFLOAT3(left, bottom,  back),	XMFLOAT2(tleft, tbottom) }, //Left bottom rear
				/*6*/{ XMFLOAT3(left,  top, front),		XMFLOAT2(tleft, ttop) }, //Left top front
				/*7*/{ XMFLOAT3(left,  top,  back),		XMFLOAT2(tleft, ttop) }, //Left top rear
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

		TexVertex* vertices = new TexVertex[cubeVertices.size()];
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
		vertexBufferDesc.ByteWidth = sizeof(TexVertex) * cubeVertices.size();
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
	stride = sizeof(TexVertex);
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
	_textureShader->Render(36, world, _shaderResourceView);
}

void TexturedCube::Move(Vector3^ move)
{
	_location->Plus(move);
}

void TexturedCube::MoveTo(Vector3^ location)
{
	_location = location;
}

void TexturedCube::ChangeRotationPoint(RotationPoint rotationPoint)
{
	_rotationPoint = rotationPoint;
}

CoreTypes::Vector3^ TexturedCube::Location()
{
	return _location;
}

CoreTypes::Vector3^ TexturedCube::Rotation()
{
	return _rotation;
}

TexturedCube::~TexturedCube()
{
	delete[] _vertexBuffer;
	delete[] _indexBuffer;
	delete[] _imageData;
	delete _texture;
	delete _shaderResourceView;
}