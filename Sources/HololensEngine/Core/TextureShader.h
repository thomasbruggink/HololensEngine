#pragma once

#include "pch.h"
#include <string>

namespace HololensEngineCore
{
	ref class HoloDrawEngine;
	
	ref class TextureShader sealed
	{
	public:
		TextureShader(HoloDrawEngine^ drawEngine);
		void Initialize();
	internal:
		void Render(int indexCount, DirectX::XMMATRIX world, ID3D11ShaderResourceView* resource, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT4 ambientColor);
	private:
		struct MatrixBufferType
		{
			DirectX::XMMATRIX world;
		};

		struct LightBufferType
		{
			DirectX::XMFLOAT4 ambientColor;
			DirectX::XMFLOAT4 diffuseColor;
			DirectX::XMFLOAT3 lightDirection;
			float padding;
		};

		ID3D11Device* _device;
		ID3D11DeviceContext* _context;
		ID3D11VertexShader* _vertexShader;
		ID3D11GeometryShader* _geometryShader;
		ID3D11PixelShader* _pixelShader;
		ID3D11InputLayout* _inputLayout;
		ID3D11Buffer* _matrixBuffer;
		ID3D11Buffer* _lightBuffer;
		ID3D11SamplerState* _samplerState;
		bool _supportsVprt;

		void SetShaderParameters(DirectX::XMMATRIX worldMatrix, ID3D11ShaderResourceView* resource, DirectX::XMFLOAT3 lightDirection, DirectX::XMFLOAT4 diffuseColor, DirectX::XMFLOAT4 ambientColor);
		void RenderShader(int indexCount);
		Platform::Array<byte>^ LoadShaderFile(std::string file);
		~TextureShader();
	};
}