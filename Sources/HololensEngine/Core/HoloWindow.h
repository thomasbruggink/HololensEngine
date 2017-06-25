#pragma once

#include "pch.h"
#include "HoloDrawEngine.h"

namespace HololensEngineCore
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class HoloWindow sealed
	{
	public:
		HoloWindow();
		void AttachWindow();
		void Initialize();
		void InitializeDrawing();
		void UninitilizeDrawing();
		void Update();
		void UpdateInput();
		void GetWindowSize(CoreTypes::Vector2^ out);
		int GetGameTime();
		HoloDrawEngine^ GetDrawEngine();
		HoloInput^ GetInput();
		void Exit();
		bool CanDraw();
		bool IsRunning();

		void SetCursorState(bool state);
		void SetCursorPosition(int x, int y);
		void SetWindow(Windows::UI::Core::CoreWindow^ coreWindow, Windows::Graphics::Holographic::HolographicSpace^ holographicSpace);
	private:
		Windows::Graphics::Holographic::HolographicSpace^ _holographicSpace;
		HoloDrawEngine^ _drawEngine;
		HoloInput^ _input;
		bool _running, _windows_visible, _cursorState;
		int _cursorX, _cursorY;
		float _windowScale;
		Platform::Agile<Windows::UI::Core::CoreWindow> _coreWindow;
		LONGLONG _frequency, _start_time;
	};
}