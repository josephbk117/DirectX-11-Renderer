#pragma once
#include "Window.h"
#include "EngineTimer.h"
#include "Imgui/ImguiManager.h"

class App
{
public:
	App();
	// master frame / message loop
	int Start();
	~App();
private:
	void DoFrame();
private:
	ImguiManager imgui;
	Window wnd;
	EngineTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<std::unique_ptr<class BaseModel>> models;
	std::vector<std::unique_ptr<class Terrain>> terrains;
};