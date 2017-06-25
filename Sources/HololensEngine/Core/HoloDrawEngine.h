#pragma once

#include "pch.h"
#include "HoloInput.h"

struct ViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 viewProjection[2];
};

namespace HololensEngineCore
{
	ref class ColorShader;

	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class HoloDrawEngine sealed
	{
	public:
		HoloDrawEngine(Windows::Graphics::Holographic::HolographicSpace^ holographicSpace, HoloInput^ input);
		float GetScreenWidth();
		float GetScreenHeight();
		void Begin();
		void End();
		void ChangeResolution(int resolution);
		void SetScissorArea(float x, float y, float width, float height);
		void RemoveScissorArea();
	internal:
		ID3D11Device4* GetDevice();
		ID3D11DeviceContext3* GetContext();
		Windows::Perception::PerceptionTimestamp^ GetCurrentPoseTimeStamp();
		ColorShader^ GetColorShader();
		DirectX::XMMATRIX* GetWorldMatrix();

		//Events
		void SetCamera(Windows::Graphics::Holographic::HolographicCamera^ holographicCamera);
	private:
		float _screenWidth;
		float _screenHeight;
		float _screenScale;
		HoloInput^ _input;

		Microsoft::WRL::ComPtr<ID3D11Device4> _device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext3> _context;
		Microsoft::WRL::ComPtr<IDXGIDevice3> _dxgiDevice;
		Microsoft::WRL::ComPtr<ID3D11BlendState> _blendState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizer;
		Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice^ _interopDevice;
		Windows::Graphics::Holographic::HolographicSpace^ _holographicSpace;
		ColorShader^ _colorShader;
		DirectX::XMMATRIX _world;
		Windows::Graphics::Holographic::HolographicFrame^ _holographicFrame;
		Windows::Graphics::Holographic::HolographicFramePrediction^ _holographicFramePrediction;

		//Camera
		Windows::Graphics::Holographic::HolographicCamera^ _holographicCamera;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _renderTarget;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _stencilView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _backBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _projectionBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> _viewProjectionBuffer;
		Windows::Foundation::Size _renderTargetSize;
		CD3D11_VIEWPORT _viewPort;
	};
}