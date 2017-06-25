#include "pch.h"

#include "AppFrameworkView.h"
#include "Engine.h"
#include "HoloWindow.h"

using namespace CoreTypes;
using namespace HololensEngineCore;

Engine::Engine()
{
}

void Engine::Run()
{
	_running = true;
	_holoWindow->Initialize();
	_holoWindow->InitializeDrawing();
	_drawEngine = _holoWindow->GetDrawEngine();
	_currentScene->Load();

	double gametime = (double)_holoWindow->GetGameTime();
	double waittime = (double)(1000 / (double)_fps);
	double diff = waittime;
	while (_running && _holoWindow->IsRunning())
	{
		if (_newScene != nullptr)
		{
			delete _currentScene;
			_currentScene = _newScene;
			_newScene = nullptr;
			_newScene->Load();
			diff = waittime;
		}

		//Call the updates in order
		UpdateWindow(diff);
		UpdateInput(diff);
		UpdateScene(diff);
		Draw(diff);

		//Get time
		double newtime = (double)_holoWindow->GetGameTime();
		diff = newtime - gametime;

		//Spin if we're going too fast
		int looprounds = 0;
		while (diff < waittime)
		{
			//Draw(diff);
			newtime = (double)_holoWindow->GetGameTime();
			diff = newtime - gametime;
			looprounds++;
		}
		_looprounds = looprounds;
		gametime = newtime;
	}
}

void Engine::Start(IScene^ scene)
{
	_running = false;
	_fps = 60;
	_currentfps = 0;
	_totalfps = 0;
	_counter = 0;
	_inputtime = 0.0;
	_looprounds = 0;
	_scenetime = 0.0;
	_windowtime = 0.0;
	_drawtime = 0.0;
	_currentScene = scene;
	AppFrameworkView::EngineCallback = this;
	_holoWindow = ref new HoloWindow();
	_holoWindow->AttachWindow();
}

HoloWindow^ Engine::GetWindow()
{
	return _holoWindow;
}

void Engine::SwitchScene(CoreTypes::IScene^ scene)
{
	_newScene = scene;
}

void Engine::UpdateWindow(double gametime)
{
	double time = _holoWindow->GetGameTime();
	_holoWindow->Update();
	_windowtime = _holoWindow->GetGameTime() - time;
}

void Engine::UpdateInput(double gametime)
{
	double time = _holoWindow->GetGameTime();
	_holoWindow->UpdateInput();
	_inputtime = _holoWindow->GetGameTime() - time;
}

void Engine::UpdateScene(double gametime)
{
	double time = _holoWindow->GetGameTime();
	_currentScene->Update(gametime);
	_scenetime = _holoWindow->GetGameTime() - time;
}

void Engine::Draw(double gametime)
{
	if (!_holoWindow->CanDraw())
		return;
	double time = _holoWindow->GetGameTime();
	_drawEngine->Begin();
	_currentScene->Draw();
	_drawEngine->End();
	_drawtime = _holoWindow->GetGameTime() - time;
	_totalfps += gametime;
	_counter++;
	if (_totalfps > 1000)
	{
		_currentfps = (int)std::ceil(1000 / (_totalfps / _counter));
		_counter = 0;
		_totalfps = 0;
	}
}