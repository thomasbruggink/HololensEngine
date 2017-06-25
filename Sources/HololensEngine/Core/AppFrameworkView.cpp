#include "pch.h"
#include "Engine.h"
#include "AppFrameworkView.h"

using namespace HololensEngineCore;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Holographic;
using namespace Windows::UI::Input::Spatial;
using namespace Platform;
using namespace DirectX;

Engine^ AppFrameworkView::EngineCallback;

void AppFrameworkView::Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ appView)
{
	appView->Activated += ref new TypedEventHandler<Windows::ApplicationModel::Core::CoreApplicationView^, IActivatedEventArgs^>(this, &AppFrameworkView::OnActivated);
}

void AppFrameworkView::SetWindow(CoreWindow^ coreWindow)
{
	HolographicSpace^ holographicSpace = HolographicSpace::CreateForCoreWindow(coreWindow);
	EngineCallback->GetWindow()->SetWindow(coreWindow, holographicSpace);
	holographicSpace->CameraAdded += ref new Windows::Foundation::TypedEventHandler<Windows::Graphics::Holographic::HolographicSpace ^, Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs ^>(this, &AppFrameworkView::OnCameraAdded);
}

void AppFrameworkView::Load(String^ entryPoint)
{
}

void AppFrameworkView::OnPointerPressed(CoreWindow ^ coreWindow, PointerEventArgs ^ pointerEventArgs)
{
	EngineCallback->GetWindow()->SetCursorState(true);
}

void AppFrameworkView::OnPointerReleased(CoreWindow ^ coreWindow, PointerEventArgs ^ pointerEventArgs)
{
	EngineCallback->GetWindow()->SetCursorState(false);
}

void AppFrameworkView::OnSizeChanged(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::WindowSizeChangedEventArgs ^ eventArgs)
{
	//Engine->GetDrawEngine()->SetScreenSize(eventArgs->Size.Width * Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel, eventArgs->Size.Height * Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel);
}

void AppFrameworkView::OnPointerMoved(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ eventArgs)
{
}

void AppFrameworkView::OnKeyDown(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::KeyEventArgs ^ eventArgs)
{
}

void AppFrameworkView::OnKeyUp(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::KeyEventArgs ^ eventArgs)
{
}

void AppFrameworkView::OnCameraAdded(Windows::Graphics::Holographic::HolographicSpace ^ holographicSpace, Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs ^ eventArgs)
{
	auto drawEngine = EngineCallback->GetWindow()->GetDrawEngine();
	drawEngine->SetCamera(eventArgs->Camera);
}

void AppFrameworkView::Run()
{
	EngineCallback->Run();
}

void AppFrameworkView::OnSourcePressed(SpatialInteractionManager^ sender, SpatialInteractionSourceEventArgs^ args)
{
	EngineCallback->GetWindow()->SetCursorState(true);
}

void AppFrameworkView::OnSourceReleased(SpatialInteractionManager^ sender, SpatialInteractionSourceEventArgs^ args)
{
	EngineCallback->GetWindow()->SetCursorState(false);
}

void AppFrameworkView::OnSourceUpdated(SpatialInteractionManager^ sender, SpatialInteractionSourceEventArgs^ args)
{
}

void AppFrameworkView::OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ appView, IActivatedEventArgs^ args)
{
	CoreWindow^ coreWindow = appView->CoreWindow;
	appView->TitleBar->ExtendViewIntoTitleBar = true;
	auto currentView = SpatialInteractionManager::GetForCurrentView();
	currentView->SourcePressed += ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(this, &AppFrameworkView::OnSourcePressed);
	currentView->SourceReleased += ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(this, &AppFrameworkView::OnSourceReleased);
	coreWindow->PointerPressed += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(this, &AppFrameworkView::OnPointerPressed);
	coreWindow->PointerReleased += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(this, &AppFrameworkView::OnPointerReleased);
	coreWindow->KeyDown += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &AppFrameworkView::OnKeyDown);
	coreWindow->KeyUp += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &AppFrameworkView::OnKeyUp);
	coreWindow->SizeChanged += ref new TypedEventHandler<CoreWindow ^, WindowSizeChangedEventArgs ^>(this, &AppFrameworkView::OnSizeChanged);
	coreWindow->PointerMoved += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(this, &AppFrameworkView::OnPointerMoved);
	coreWindow->Activate();
}

void AppFrameworkView::Uninitialize()
{

}