#include "ColorShader.h"
#include "HoloDrawEngine.h"
#include <fstream>

using namespace Platform;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace HololensEngineCore;

ColorShader::ColorShader(HoloDrawEngine^ drawEngine)
{
	_device = drawEngine->GetDevice();
	_context = drawEngine->GetContext();
	_samplerState = nullptr;
	_matrixBuffer = nullptr;
	_supportsVprt = false;
}

void ColorShader::Initialize()
{
	// Check for device support for the optional feature that allows setting the render target array index from the vertex shader stage.
	D3D11_FEATURE_DATA_D3D11_OPTIONS3 options;
	_device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS3, &options, sizeof(options));
	if (options.VPAndRTArrayIndexFromAnyShaderFeedingRasterizer)
		_supportsVprt = true;

	// load the shader files
	Array<byte>^ vertexShaderFile = _supportsVprt ? LoadShaderFile("Core\\ColorVprtVertexShader.cso") : LoadShaderFile("Core\\ColorVertexShader.cso");
	Array<byte>^ pixelShaderFile = LoadShaderFile("Core\\ColorPixelShader.cso");

	// create the shader objects
	_device->CreateVertexShader(vertexShaderFile->Data, vertexShaderFile->Length, nullptr, &_vertexShader);
	_device->CreatePixelShader(pixelShaderFile->Data, pixelShaderFile->Length, nullptr, &_pixelShader);

	// set the shader objects as the active shaders
	_context->VSSetShader(_vertexShader, nullptr, 0);
	_context->PSSetShader(_pixelShader, nullptr, 0);

	if (!_supportsVprt)
	{
		Array<byte>^ geometryShaderFile = LoadShaderFile("Core\\ColorGeometryShader.cso");
		_device->CreateGeometryShader(geometryShaderFile->Data, geometryShaderFile->Length, nullptr, &_geometryShader);
		_context->GSSetShader(_geometryShader, nullptr, 0);
	}

	// initialize input layout
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// create and set the input layout
	_device->CreateInputLayout(ied, sizeof(ied) / sizeof(ied[0]), vertexShaderFile->Data, vertexShaderFile->Length, &_inputLayout);
	_context->IASetInputLayout(_inputLayout);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	_device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBuffer);

	D3D11_BUFFER_DESC lightBufferDesc;
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	_device->CreateBuffer(&lightBufferDesc, NULL, &_lightBuffer);

	//Setup texture sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	//Create the sampler state
	_device->CreateSamplerState(&samplerDesc, &_samplerState);
}

void ColorShader::Render(int indexCount, DirectX::XMMATRIX world, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT4 ambientColor)
{
	SetShaderParameters(world, lightDirection, diffuseColor, ambientColor);
	RenderShader(indexCount);
}

void ColorShader::SetShaderParameters(DirectX::XMMATRIX worldMatrix, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT4 ambientColor)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* lightDataPtr;

	worldMatrix = XMMatrixTranspose(worldMatrix);

	_context->Map(_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;

	// Unlock the constant buffer.
	_context->Unmap(_matrixBuffer, 0);

	// Set the constant buffer in the vertex shader with the updated values.
	//Write to buffer 0
	_context->VSSetConstantBuffers(0, 1, &_matrixBuffer);

	//Map the light buffer so we can write
	_context->Map(_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//Get the pointer
	lightDataPtr = (LightBufferType*)mappedResource.pData;
	//Copy the variables into the constant buffer
	lightDataPtr->ambientColor = ambientColor;
	lightDataPtr->diffuseColor = diffuseColor;
	lightDataPtr->lightDirection = lightDirection;
	lightDataPtr->padding = 0.0f;

	//Unlock the light constant buffer
	_context->Unmap(_lightBuffer, 0);

	//Set the vertex shader constant buffer
	_context->PSSetConstantBuffers(0, 1, &_lightBuffer);
}

void ColorShader::RenderShader(int indexCount)
{
	_context->IASetInputLayout(_inputLayout);
	//Set the shaders
	_context->VSSetShader(_vertexShader, NULL, 0);
	if (!_supportsVprt)
		_context->GSSetShader(_geometryShader, NULL, 0);
	_context->PSSetShader(_pixelShader, NULL, 0);

	//Set the sampler
	_context->PSSetSamplers(0, 1, &_samplerState);

	//Render
	//_context->DrawIndexed(indexCount, 0, 0);
	_context->DrawIndexedInstanced(
		indexCount,   // Index count per instance.
		2,              // Instance count.
		0,              // Start index location.
		0,              // Base vertex location.
		0               // Start instance location.
	);
}

Array<byte>^ ColorShader::LoadShaderFile(std::string file)
{
	String^ filePath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
	std::wstring folderNameW(filePath->Begin());
	std::string folderNameA(folderNameW.begin(), folderNameW.end());

	file = folderNameA + "\\" + file;
	Array<byte>^ data = nullptr;

	// open the file
	std::ifstream VertexFile(file, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
	if (VertexFile.is_open())
	{
		// find the length of the file
		int length = (int)VertexFile.tellg();

		// collect the file data
		data = ref new Array<byte>(length);
		VertexFile.seekg(0, std::ios::beg);
		VertexFile.read(reinterpret_cast<char*>(data->Data), length);
		VertexFile.close();
	}

	return data;
}

ColorShader::~ColorShader()
{
	_vertexShader->Release();
	_pixelShader->Release();
	_inputLayout->Release();
	_matrixBuffer->Release();
	_lightBuffer->Release();
	_samplerState->Release();
	if (_geometryShader != nullptr)
		_geometryShader->Release();
}