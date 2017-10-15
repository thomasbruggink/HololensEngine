#pragma once

#include "pch.h"
#include <vector>

namespace HololensEngineCore
{
	ref class TextureShader;
	ref class HoloDrawEngine;

	struct TexVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Tex;
		DirectX::XMFLOAT3 Norm;
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class TexturedCube sealed
	{
	public:
		TexturedCube(CoreTypes::Vector3^ location, CoreTypes::CubeSize^ size, HoloDrawEngine^ drawEngine, CoreTypes::ImageData^ imageData);
		void Draw();
		void Move(CoreTypes::Vector3^ move);
		void MoveTo(CoreTypes::Vector3^ location);
		void ChangeRotationPoint(CoreTypes::RotationPoint rotationPoint);
		CoreTypes::Vector3^ Location();
		CoreTypes::Vector3^ Rotation();
		CoreTypes::Vector3^ LightDirection();
		CoreTypes::Vector3^ Scale();
		float AmbientLight();
		void AmbientLight(float input);
	private:
		ID3D11Device* _device;
		ID3D11DeviceContext* _context;
		ID3D11Buffer *_vertexBuffer, *_indexBuffer;
		TextureShader^ _textureShader;
		ID3D11Texture2D* _texture;
		ID3D11ShaderResourceView* _shaderResourceView;
		unsigned char* _imageData;
		CoreTypes::Vector3^ _rotation;
		CoreTypes::RotationPoint _rotationPoint;

		CoreTypes::CubeSize^ _size;
		CoreTypes::Vector3^ _location;
		CoreTypes::Vector3^ _lightDirection;
		CoreTypes::Vector3^ _scale;
		float _ambientLight;
		~TexturedCube();

		void ConvertImageData(CoreTypes::ImageData^ imageData);
	};
}