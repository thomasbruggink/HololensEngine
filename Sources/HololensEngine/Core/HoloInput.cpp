#include "pch.h"
#include "HoloInput.h"

using namespace HololensEngineCore;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Input::Spatial;
using namespace Windows::Perception;
using namespace Windows::Perception::Spatial;

HoloInput::HoloInput()
{
	_locator = SpatialLocator::GetDefault();
	_inputManager = SpatialInteractionManager::GetForCurrentView();
	_reference = _locator->CreateStationaryFrameOfReferenceAtCurrentLocation();
}

bool HoloInput::IsKeyDown(Windows::System::VirtualKey key)
{
	return false;
}

bool HoloInput::Click()
{
	return false;
}

CoreTypes::Vector2 ^  HoloInput::GetCursorPosition()
{
	return _cursorLocation;
}

void HoloInput::Update()
{
	_coordinateSystem = _reference->CoordinateSystem;
}

void HoloInput::Update(int offset, bool state)
{
	throw ref new Platform::NotImplementedException();
}

void HoloInput::UpdateCursor(float x, float y, bool click)
{
	throw ref new Platform::NotImplementedException();
}

Windows::UI::Input::Spatial::SpatialPointerPose^ HoloInput::GetCurrentPointerPose(Windows::Perception::PerceptionTimestamp ^ timestamp)
{
	if (timestamp == nullptr)
		return nullptr;
	return SpatialPointerPose::TryGetAtTimestamp(_coordinateSystem, timestamp);
}

Windows::Perception::Spatial::SpatialCoordinateSystem^ HoloInput::GetCoordinateSystem()
{
	return _coordinateSystem;
}

void HoloInput::ResetReferencePoint()
{
	_locator = SpatialLocator::GetDefault();
	_inputManager = SpatialInteractionManager::GetForCurrentView();
	_reference = _locator->CreateStationaryFrameOfReferenceAtCurrentLocation();
}
