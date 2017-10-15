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
	_lightDirection = ref new Vector3(0.0f, 0.0f, 0.0f);
	_scale = ref new Vector3(1.0f, 1.0f, 1.0f);
	_ambientLight = 1.0f;

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

		std::array<TexVertex, 36> cubeVertices =
		{ {
			{ XMFLOAT3(left,  bottom, back), XMFLOAT2(tleft, ttop),  XMFLOAT3(0.0,  0.0, back) },
			{ XMFLOAT3(right,  bottom, back), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0,  0.0, back) },
			{ XMFLOAT3(left, top, back), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0,  0.0, back) },
			{ XMFLOAT3(left, top, back), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0,  0.0, back) },
			{ XMFLOAT3(right,  bottom, back), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0,  0.0, back) },
			{ XMFLOAT3(right, top, back), XMFLOAT2(tright, tbottom),  XMFLOAT3(0.0,  0.0, back) },
			{ XMFLOAT3(right,  bottom, back), XMFLOAT2(tleft, ttop),  XMFLOAT3(right,  0.0,  0.0) },
			{ XMFLOAT3(right,  bottom,  front), XMFLOAT2(tright, ttop),  XMFLOAT3(right,  0.0,  0.0) },
			{ XMFLOAT3(right, top, back), XMFLOAT2(tleft, tbottom),  XMFLOAT3(right,  0.0,  0.0) },
			{ XMFLOAT3(right, top, back), XMFLOAT2(tleft, tbottom),  XMFLOAT3(right,  0.0,  0.0) },
			{ XMFLOAT3(right,  bottom,  front), XMFLOAT2(tright, ttop),  XMFLOAT3(right,  0.0,  0.0) },
			{ XMFLOAT3(right, top,  front), XMFLOAT2(tright, tbottom),  XMFLOAT3(right,  0.0,  0.0) },
			{ XMFLOAT3(right,  bottom,  front), XMFLOAT2(tleft, ttop),  XMFLOAT3(0.0,  0.0,  front) },
			{ XMFLOAT3(left,  bottom,  front), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0,  0.0,  front) },
			{ XMFLOAT3(right, top,  front), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0,  0.0,  front) },
			{ XMFLOAT3(right, top,  front), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0,  0.0,  front) },
			{ XMFLOAT3(left,  bottom,  front), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0,  0.0,  front) },
			{ XMFLOAT3(left, top,  front), XMFLOAT2(tright, tbottom),  XMFLOAT3(0.0,  0.0,  front) },
			{ XMFLOAT3(left,  bottom,  front), XMFLOAT2(tleft, ttop),  XMFLOAT3(left,  0.0,  0.0) },
			{ XMFLOAT3(left,  bottom, back), XMFLOAT2(tright, ttop),  XMFLOAT3(left,  0.0,  0.0) },
			{ XMFLOAT3(left, top,  front), XMFLOAT2(tleft, tbottom),  XMFLOAT3(left,  0.0,  0.0) },
			{ XMFLOAT3(left, top,  front), XMFLOAT2(tleft, tbottom),  XMFLOAT3(left,  0.0,  0.0) },
			{ XMFLOAT3(left,  bottom, back), XMFLOAT2(tright, ttop),  XMFLOAT3(left,  0.0,  0.0) },
			{ XMFLOAT3(left, top, back), XMFLOAT2(tright, tbottom),  XMFLOAT3(left,  0.0,  0.0) },
			{ XMFLOAT3(left,  bottom,  front), XMFLOAT2(tleft, ttop),  XMFLOAT3(0.0,  bottom,  0.0) },
			{ XMFLOAT3(right,  bottom,  front), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0,  bottom,  0.0) },
			{ XMFLOAT3(left,  bottom, back), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0,  bottom,  0.0) },
			{ XMFLOAT3(left,  bottom, back), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0,  bottom,  0.0) },
			{ XMFLOAT3(right,  bottom,  front), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0,  bottom,  0.0) },
			{ XMFLOAT3(right,  bottom, back), XMFLOAT2(tright, tbottom),  XMFLOAT3(0.0,  bottom,  0.0) },
			{ XMFLOAT3(left, top, back), XMFLOAT2(tleft, ttop),  XMFLOAT3(0.0, top,  0.0) },
			{ XMFLOAT3(right, top, back), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0, top,  0.0) },
			{ XMFLOAT3(left, top,  front), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0, top,  0.0) },
			{ XMFLOAT3(left, top,  front), XMFLOAT2(tleft, tbottom),  XMFLOAT3(0.0, top,  0.0) },
			{ XMFLOAT3(right, top, back), XMFLOAT2(tright, ttop),  XMFLOAT3(0.0, top,  0.0) },
			{ XMFLOAT3(right, top,  front), XMFLOAT2(tright, tbottom),  XMFLOAT3(0.0, top,  0.0) }
		} };

		TexVertex* vertices = new TexVertex[cubeVertices.size()];
		unsigned int indexCount = 36;
		unsigned long * indices = new unsigned long[indexCount];

		for (unsigned int i = 0; i < cubeVertices.size(); i++)
		{
			vertices[i] = cubeVertices[i];
		}

		for (unsigned int i = 0; i < indexCount; i++)
		{
			indices[i] = i;
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
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
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
	world *= XMMatrixScaling(_scale->X, _scale->Y, _scale->Z);
	world *= XMMatrixTranslation(_location->X, _location->Y, _location->Z);

	//Draw texture
	_textureShader->Render(36, world, _shaderResourceView, DirectX::XMFLOAT3(_lightDirection->X, _lightDirection->Y, _lightDirection->Z), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(_ambientLight, _ambientLight, _ambientLight, 1.0f));
}

void TexturedCube::Move(Vector3^ move)
{
	_location = _location->Plus(move);
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

CoreTypes::Vector3^ TexturedCube::LightDirection()
{
	return _lightDirection;
}

CoreTypes::Vector3^ TexturedCube::Scale()
{
	return _scale;
}

float TexturedCube::AmbientLight()
{
	return _ambientLight;
}

void TexturedCube::AmbientLight(float input)
{
	_ambientLight = input;
}

TexturedCube::~TexturedCube()
{
	delete[] _vertexBuffer;
	delete[] _indexBuffer;
	delete[] _imageData;
	delete _texture;
	delete _shaderResourceView;
}