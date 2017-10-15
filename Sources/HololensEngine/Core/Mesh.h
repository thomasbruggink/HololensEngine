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
	public ref class Mesh sealed
	{
	public:
		Mesh(CoreTypes::Vector3^ location, CoreTypes::ObjReader^ reader, HoloDrawEngine^ drawEngine);
		Mesh(CoreTypes::Vector3^ location, CoreTypes::ObjReader^ reader, HoloDrawEngine^ drawEngine, CoreTypes::Color^ color);
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
		CoreTypes::Vector3^ _rotation;
		CoreTypes::RotationPoint _rotationPoint;
		ColorShader^ _colorShader;
		unsigned long _vertexCount;
		Vertex* _vertice;
		unsigned long _indexCount;
		unsigned long* _indices;

		CoreTypes::CubeSize^ _size;
		CoreTypes::Vector3^ _location;
		CoreTypes::Vector3^ _lightDirection;
		CoreTypes::Vector3^ _scale;
		float _ambientLight;

		void Init(CoreTypes::Vector3^ location, CoreTypes::ObjReader^ reader, HoloDrawEngine^ drawEngine, CoreTypes::Color^ color);
		~Mesh();
	};
}