#pragma once

#include "pch.h"
#include "HoloWindow.h"

namespace HololensEngineCore
{
	ref class Engine;

	[Windows::Foundation::Metadata::WebHostHidden]
	ref class AppFrameworkView sealed : public Windows::ApplicationModel::Core::IFrameworkView
	{
	public:
		virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ appView);
		virtual void SetWindow(Windows::UI::Core::CoreWindow^ coreWindow);
		virtual void Load(Platform::String^ entryPoint);
		virtual void Run();
		virtual void Uninitialize();

		void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ appView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
		void OnPointerPressed(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ pointerEventArgs);
		void OnPointerReleased(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ pointerEventArgs);
		void OnSizeChanged(Windows::UI::Core::CoreWindow ^, Windows::UI::Core::WindowSizeChangedEventArgs ^);
		void OnPointerMoved(Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^);
		void OnKeyDown(Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^);
		void OnKeyUp(Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^);
		void OnSourcePressed(Windows::UI::Input::Spatial::SpatialInteractionManager^ sender, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args);
		void OnSourceReleased(Windows::UI::Input::Spatial::SpatialInteractionManager^ sender, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args);
		void OnSourceUpdated(Windows::UI::Input::Spatial::SpatialInteractionManager^ sender, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args);
		void OnCameraAdded(Windows::Graphics::Holographic::HolographicSpace ^, Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs ^);
	internal:
		static Engine^ EngineCallback;
	private:
		bool _shift;
	};
}