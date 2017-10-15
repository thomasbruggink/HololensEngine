#pragma once

#include "pch.h"
#include <vector>

namespace HololensEngineCore
{
	ref class ColorShader;
	ref class HoloDrawEngine;

	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT3 Norm;
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class ColoredCube sealed
	{
	public:
		ColoredCube(CoreTypes::Vector3^ location, CoreTypes::CubeSize^ size, HoloDrawEngine^ drawEngine);
		ColoredCube(CoreTypes::Vector3^ location, CoreTypes::CubeSize^ size, CoreTypes::Color^ color, HoloDrawEngine^ drawEngine);
		void Draw();
		void Move(CoreTypes::Vector3^ move);
		void MoveTo(CoreTypes::Vector3^ location);
		void ChangeColor(CoreTypes::Color^ color);
		void ChangeColor(int position, CoreTypes::Color^ color);
		void ChangeRotationPoint(CoreTypes::RotationPoint rotationPoint);
		CoreTypes::Vector3^ Location();
		CoreTypes::Vector3^ Rotation();
	private:
		ID3D11Device* _device;
		ID3D11DeviceContext* _context;
		ID3D11Buffer *_vertexBuffer, *_indexBuffer;
		ColorShader^ _colorShader;
		CoreTypes::Vector3^ _rotation;
		CoreTypes::RotationPoint _rotationPoint;

		CoreTypes::CubeSize^ _size;
		CoreTypes::Vector3^ _location;
		std::vector<CoreTypes::Color^> _color;
		CoreTypes::Vector3^ _lightDirection;
		CoreTypes::Vector3^ _scale;
		~ColoredCube();

		void Construct(CoreTypes::Vector3^ location, CoreTypes::CubeSize^ size, CoreTypes::Color^ color, HoloDrawEngine^ drawEngine);
	};
}