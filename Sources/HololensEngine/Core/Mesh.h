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
		//DirectX::XMFLOAT3 Norm;
		DirectX::XMFLOAT4 Color;
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Mesh sealed
	{
	public:
		Mesh(CoreTypes::Vector3^ location, CoreTypes::ObjReader^ reader, HoloDrawEngine^ drawEngine);
		void Draw();
		void Move(CoreTypes::Vector3^ move);
		void MoveTo(CoreTypes::Vector3^ location);
		void ChangeRotationPoint(CoreTypes::RotationPoint rotationPoint);
		CoreTypes::Vector3^ Location();
		CoreTypes::Vector3^ Rotation();
	private:
		ID3D11Device* _device;
		ID3D11DeviceContext* _context;
		ID3D11Buffer *_vertexBuffer, *_indexBuffer;
		CoreTypes::Vector3^ _rotation;
		CoreTypes::RotationPoint _rotationPoint;
		ColorShader^ _colorShader;
		unsigned long _vertexCount;
		Vertex* _vertice;
		unsigned long _indexCount;
		unsigned long* _indices;

		CoreTypes::CubeSize^ _size;
		CoreTypes::Vector3^ _location;
		~Mesh();
	};
}