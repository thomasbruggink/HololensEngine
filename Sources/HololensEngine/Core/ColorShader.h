#pragma once

#include "pch.h"
#include <string>

namespace HololensEngineCore
{
	ref class HoloDrawEngine;

	ref class ColorShader sealed
	{
		public:
			ColorShader(HoloDrawEngine^ drawEngine);
			void Initialize();
		internal:
			void Render(int indexCount, DirectX::XMMATRIX world);
		private:
			struct MatrixBufferType
			{
				DirectX::XMMATRIX world;
			};

			ID3D11Device* _device;
			ID3D11DeviceContext* _context;
			ID3D11VertexShader* _vertexShader;
			ID3D11GeometryShader* _geometryShader;
			ID3D11PixelShader* _pixelShader;
			ID3D11InputLayout* _inputLayout;
			ID3D11Buffer* _matrixBuffer;
			ID3D11SamplerState* _samplerState;
			bool _supportsVprt;

			void SetShaderParameters(DirectX::XMMATRIX worldMatrix);
			void RenderShader(int indexCount);
			Platform::Array<byte>^ LoadShaderFile(std::string file);
			~ColorShader();
	};
}