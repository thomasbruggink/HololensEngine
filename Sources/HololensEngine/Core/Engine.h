#pragma once

#include "HoloWindow.h"
#include "HoloDrawEngine.h"

namespace HololensEngineCore
{
	//The engine controls the gameloop and calls the attached scene
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Engine sealed
	{
		public:
			Engine();
			//Start the engine
			void Start(CoreTypes::IScene^ scene);
			//Change to a new scene
			void SwitchScene(CoreTypes::IScene^ scene);
			//Get the Main window
			HoloWindow^ GetWindow();

		internal:
			void Run();
		private:
			bool _running;
			int _fps;
			int _looprounds;
			double _totalfps, _windowtime, _inputtime, _scenetime, _drawtime;
			int _currentfps, _counter;
			CoreTypes::IScene^ _currentScene;
			CoreTypes::IScene^ _newScene;
			HoloWindow^ _holoWindow;
			HoloDrawEngine^ _drawEngine;
			
			void UpdateWindow(double gamtime);
			void UpdateInput(double gamtime);
			void UpdateScene(double gamtime);
			void Draw(double gametime);
	};
}