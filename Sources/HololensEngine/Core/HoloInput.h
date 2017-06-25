#pragma once

#include "pch.h"
#include <Xinput.h>

namespace HololensEngineCore
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class HoloInput sealed
	{
	public:
		HoloInput();
		bool IsKeyDown(Windows::System::VirtualKey key);
		bool Click();
		CoreTypes::Vector2^ GetCursorPosition();
		void Update();
		void Update(int offset, bool state);
		void UpdateCursor(float x, float y, bool click);

		Windows::UI::Input::Spatial::SpatialPointerPose^ GetCurrentPointerPose(Windows::Perception::PerceptionTimestamp^ timestamp);
		Windows::Perception::Spatial::SpatialCoordinateSystem^ GetCoordinateSystem();
		void ResetReferencePoint();

	private:
		CoreTypes::Vector2^ _cursorLocation;
		XINPUT_CAPABILITIES _capabilities;
		XINPUT_STATE _controllerState;

		Windows::UI::Input::Spatial::SpatialInteractionManager^ _inputManager;
		Windows::Perception::Spatial::SpatialLocator^ _locator;
		Windows::Perception::Spatial::SpatialCoordinateSystem^ _coordinateSystem;
		Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ _reference;
	};
}