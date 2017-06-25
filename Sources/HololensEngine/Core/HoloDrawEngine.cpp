#include "HoloDrawEngine.h"
#include "ColorShader.h"

#include <Collection.h>
#include <windows.graphics.directx.direct3d11.interop.h>

using namespace HololensEngineCore;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace DirectX;
using namespace Windows::Perception;
using namespace Windows::Perception::Spatial;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Graphics::Holographic;

HoloDrawEngine::HoloDrawEngine(Windows::Graphics::Holographic::HolographicSpace ^ holographicSpace, HoloInput ^ input)
{
	_holographicSpace = holographicSpace;
	_input = input;

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	CoreWindow^ coreWindow = CoreWindow::GetForCurrentThread(); //get the window pointer
	float width = coreWindow->Bounds.Width * (float)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;
	float height = coreWindow->Bounds.Height * (float)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;

	auto newDeviceResult = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&device,
		nullptr,
		&context);
	device.As(&_device);
	context.As(&_context);

	//obtain the DXGI factory
	ComPtr<IDXGIDevice3> dxgiDevice;
	ComPtr<IDXGIAdapter> dxgiAdapter;
	ComPtr<IDXGIFactory2> dxgiFactory;

	device.As(&dxgiDevice);

	//Setup blending
	ComPtr<ID3D11BlendState> d3dBlendState;
	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable = true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	_device->CreateBlendState(&omDesc, &d3dBlendState);//ffffffff
	_context->OMSetBlendState(d3dBlendState.Get(), 0, 0xffffffff);
	d3dBlendState.As(&_blendState);

	//Setup rasterizer
	// Setup the raster description which will determine how and what polygons will be drawn.
	ComPtr<ID3D11RasterizerState> rasterizerSate;
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	auto result = _device->CreateRasterizerState(&rasterDesc, &rasterizerSate);
	// Now set the rasterizer state.
	_context->RSSetState(rasterizerSate.Get());
	rasterizerSate.As(&_rasterizer);

	_screenWidth = width;
	_screenHeight = height;
	_screenScale = 1.0f;

	//Create the interop device
	_interopDevice = CreateDirect3DDevice(dxgiDevice.Get());
	//Set the direct device in the holographic space
	_holographicSpace->SetDirect3D11Device(_interopDevice);

	_colorShader = ref new ColorShader(this);
	_world = XMMatrixIdentity();
}

float HoloDrawEngine::GetScreenWidth()
{
	return _screenWidth;
}

float HoloDrawEngine::GetScreenHeight()
{
	return _screenHeight;
}

//Set up render call by getting the next frameprediction
void HoloDrawEngine::Begin()
{
	//Update projection
	_holographicFrame = _holographicSpace->CreateNextFrame();
	_holographicFramePrediction = _holographicFrame->CurrentPrediction;

	HolographicFrame^ frame = _holographicFrame;
	HolographicFramePrediction^ framePrediction = _holographicFramePrediction;

	for (HolographicCameraPose^ pose : framePrediction->CameraPoses)
	{
		//Ensure camera buffers are setup
		HolographicCameraRenderingParameters^ renderingParameters = frame->GetRenderingParameters(pose);
		IDirect3DSurface^ surface = renderingParameters->Direct3D11BackBuffer;
		ComPtr<ID3D11Resource> resource;
		GetDXGIInterfaceFromObject(surface, IID_PPV_ARGS(&resource));

		ComPtr<ID3D11Texture2D> cameraBackBuffer;
		resource.As(&cameraBackBuffer);

		if (_backBuffer.Get() != cameraBackBuffer.Get())
		{
			_backBuffer = cameraBackBuffer;
			_device->CreateRenderTargetView(_backBuffer.Get(), nullptr, &_renderTarget);
			Windows::Foundation::Size currentSize = _holographicCamera->RenderTargetSize;
			if (_renderTargetSize != currentSize)
			{
				_renderTargetSize = currentSize;
				_stencilView.Reset();
			}
		}

		if (_stencilView == nullptr)
		{
			// Create a depth stencil view for use with 3D rendering if needed.
			CD3D11_TEXTURE2D_DESC depthStencilDesc(
				DXGI_FORMAT_D16_UNORM,
				static_cast<UINT>(_renderTargetSize.Width),
				static_cast<UINT>(_renderTargetSize.Height),
				_holographicCamera->IsStereo ? 2 : 1, // Create two textures when rendering in stereo.
				1, // Use a single mipmap level.
				D3D11_BIND_DEPTH_STENCIL
			);

			ComPtr<ID3D11Texture2D> depthStencil;
			_device->CreateTexture2D(
				&depthStencilDesc,
				nullptr,
				&depthStencil
			);

			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(
				_holographicCamera->IsStereo ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D
			);
			_device->CreateDepthStencilView(
				depthStencil.Get(),
				&depthStencilViewDesc,
				&_stencilView
			);
		}

		// Create the constant buffer, if needed.
		if (_viewProjectionBuffer == nullptr)
		{
			// Create a constant buffer to store view and projection matrices for the camera.
			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
			_device->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&_viewProjectionBuffer
			);
		}

		//Setup rendering
		ID3D11RenderTargetView *const targets[1] = { _renderTarget.Get() };
		_context->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), _stencilView.Get());

		//Clear buffers
		_context->ClearRenderTargetView(_renderTarget.Get(), DirectX::Colors::Transparent);
		_context->ClearDepthStencilView(_stencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		//Render loop
		_viewPort = CD3D11_VIEWPORT(
			pose->Viewport.Left,
			pose->Viewport.Top,
			pose->Viewport.Width,
			pose->Viewport.Height
		);

		//Setup the projection
		// The projection transform for each frame is provided by the HolographicCameraPose.
		HolographicStereoTransform cameraProjectionTransform = pose->ProjectionTransform;

		// Get a container object with the view and projection matrices for the given
		// pose in the given coordinate system.
		auto coordinateSystem = _input->GetCoordinateSystem();
		Platform::IBox<HolographicStereoTransform>^ viewTransformContainer = pose->TryGetViewTransform(coordinateSystem);

		// If TryGetViewTransform returns a null pointer, that means the pose and coordinate
		// system cannot be understood relative to one another; content cannot be rendered
		// in this coordinate system for the duration of the current frame.
		// This usually means that positional tracking is not active for the current frame, in
		// which case it is possible to use a SpatialLocatorAttachedFrameOfReference to render
		// content that is not world-locked instead.
		bool viewTransformAcquired = viewTransformContainer != nullptr;
		if (viewTransformAcquired)
		{
			// Otherwise, the set of view transforms can be retrieved.
			ViewProjectionConstantBuffer viewProjectionConstantBufferData;
			HolographicStereoTransform viewCoordinateSystemTransform = viewTransformContainer->Value;

			// Update the view matrices. Holographic cameras (such as Microsoft HoloLens) are
			// constantly moving relative to the world. The view matrices need to be updated
			// every frame.
			XMStoreFloat4x4(
				&viewProjectionConstantBufferData.viewProjection[0],
				XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)&viewCoordinateSystemTransform.Left) * XMLoadFloat4x4((XMFLOAT4X4*)&cameraProjectionTransform.Left))
			);
			XMStoreFloat4x4(
				&viewProjectionConstantBufferData.viewProjection[1],
				XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)&viewCoordinateSystemTransform.Right) * XMLoadFloat4x4((XMFLOAT4X4*)&cameraProjectionTransform.Right))
			);

			_context->UpdateSubresource(
				_viewProjectionBuffer.Get(),
				0,
				nullptr,
				&viewProjectionConstantBufferData,
				0,
				0
			);
		}

		_context->RSSetViewports(1, &_viewPort);

		// Send the constant buffer to the vertex shader.
		_context->VSSetConstantBuffers(
			1,
			1,
			_viewProjectionBuffer.GetAddressOf()
		);
	}
}

void HoloDrawEngine::End()
{
	HolographicFramePresentResult presentResult = _holographicFrame->PresentUsingCurrentPrediction();

	HolographicFramePrediction^ prediction = _holographicFrame->CurrentPrediction;

	for (auto cameraPose : prediction->CameraPoses)
	{
		// Discard the contents of the render target.
		// This is a valid operation only when the existing contents will be
		// entirely overwritten. If dirty or scroll rects are used, this call
		// should be removed.
		_context->DiscardView(_renderTarget.Get());

		// Discard the contents of the depth stencil.
		_context->DiscardView(_stencilView.Get());
	}
}

void HoloDrawEngine::ChangeResolution(int resolution)
{
	throw ref new Platform::NotImplementedException();
}

void HoloDrawEngine::SetScissorArea(float x, float y, float width, float height)
{
	throw ref new Platform::NotImplementedException();
}

void HoloDrawEngine::RemoveScissorArea()
{
	throw ref new Platform::NotImplementedException();
}

ID3D11Device4 * HoloDrawEngine::GetDevice()
{
	return _device.Get();
}

ID3D11DeviceContext3 * HoloDrawEngine::GetContext()
{
	return _context.Get();
}

Windows::Perception::PerceptionTimestamp ^ HoloDrawEngine::GetCurrentPoseTimeStamp()
{
	if (_holographicFramePrediction == nullptr)
		return nullptr;
	return _holographicFramePrediction->Timestamp;
}

ColorShader ^ HololensEngineCore::HoloDrawEngine::GetColorShader()
{
	return _colorShader;
}

DirectX::XMMATRIX * HoloDrawEngine::GetWorldMatrix()
{
	return &_world;
}

void HoloDrawEngine::SetCamera(Windows::Graphics::Holographic::HolographicCamera ^ holographicCamera)
{
	_holographicCamera = holographicCamera;
}