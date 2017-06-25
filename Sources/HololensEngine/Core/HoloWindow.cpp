#include "HoloWindow.h"
#include "HoloDrawEngine.h"
#include "HoloInput.h"
#include "AppFrameworkView.h"
#include "AppFrameworkViewSource.h"
#include "Engine.h"
#include "pch.h"

#include <DirectXMath.h>

using namespace HololensEngineCore;

using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;
using namespace DirectX;

using namespace Platform;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Graphics::Holographic;
using namespace Windows::ApplicationModel::Core;

#define WIDTH 1280
#define HEIGHT 720

HoloWindow::HoloWindow()
{
	_running = false;
	_input = nullptr;
	_cursorX = 0;
	_cursorY = 0;
	_windows_visible = false;
	_cursorState = false;
	//_audioEngine = new HololensAudioEngine();

	LARGE_INTEGER start, freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	_frequency = freq.QuadPart;
	_start_time = start.QuadPart;	
}

void HoloWindow::AttachWindow()
{
	CoreApplication::Run(ref new AppFrameworkViewSource());
}

void HoloWindow::Initialize()
{
	_windowScale = (float)Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;
	_windows_visible = true;
	_running = true;
}

void HoloWindow::SetWindow(CoreWindow^ coreWindow, HolographicSpace^ holographicSpace)
{
	_coreWindow = coreWindow;
	_holographicSpace = holographicSpace;
	_input = ref new HoloInput();
	_drawEngine = ref new HoloDrawEngine(_holographicSpace, _input);
}

void HoloWindow::InitializeDrawing()
{

}

void HoloWindow::Update()
{
	_coreWindow->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
	_input->Update();

	SpatialPointerPose^ pose = _input->GetCurrentPointerPose(_drawEngine->GetCurrentPoseTimeStamp());
	if (pose == nullptr)
		return;
	float3 headPosition = pose->Head->Position;
	float3 headDirection = pose->Head->ForwardDirection;

	float distance = 2.0f;
	FXMVECTOR to = {
		headPosition.x + (distance * headDirection.x),
		headPosition.y + (distance * headDirection.y),
		headPosition.z + (distance * headDirection.z),
	};
	FXMVECTOR from = { headPosition.x, headPosition.y, headPosition.z };

	//Calculate point on plane
	FXMVECTOR leftTop = { 0.0f, 0.0f, -3.5f };
	FXMVECTOR leftBottom = { _drawEngine->GetScreenHeight(), 0, -3.5f };
	FXMVECTOR rightTop = { 0, _drawEngine->GetScreenWidth(), -3.5f };
	//Calculate vectors
	XMVECTOR plane = XMPlaneFromPoints(leftTop, leftBottom, rightTop);
	XMVECTOR intersection = XMPlaneIntersectLine(plane, from, to);

	float x = intersection.vector4_f32[0];// *(_drawEngine->GetScreenWidth() / _drawEngine->GetScreenOfficialWidth()) * _windowScale;
	float y = intersection.vector4_f32[1];// *(_drawEngine->GetScreenHeight() / _drawEngine->GetScreenOfficialHeight()) * _windowScale;
	float half = _drawEngine->GetScreenWidth() / 2.0f;

	//_input->UpdateCursor(x, y, _cursorState);
}

void HoloWindow::GetWindowSize(CoreTypes::Vector2^ out)
{
	out->X = _drawEngine->GetScreenWidth();
	out->Y = _drawEngine->GetScreenHeight();
}

void HoloWindow::UpdateInput()
{

}

void HoloWindow::UninitilizeDrawing()
{
}

HoloInput^ HoloWindow::GetInput()
{
	return _input;
}

HoloDrawEngine ^ HoloWindow::GetDrawEngine()
{
	return _drawEngine;
}

int HoloWindow::GetGameTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return (int)((time.QuadPart - _start_time) / (_frequency / 1000));
}

void HoloWindow::Exit()
{
	_running = false;
}

bool HoloWindow::CanDraw()
{
	return _windows_visible;
}

bool HoloWindow::IsRunning()
{
	return _running;
}

void HoloWindow::SetCursorState(bool state)
{
	_cursorState = state;
}

void HoloWindow::SetCursorPosition(int x, int y)
{
	_cursorX = x;
	_cursorY = y;
}