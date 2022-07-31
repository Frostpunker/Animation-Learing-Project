#pragma once
#include"nuklear.h"
class Application
{
private:
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

public:
	inline Application() {}
	inline virtual ~Application() {}
	inline virtual void Init() {}
	inline virtual void Update(float inDeltaTime) {}
	inline virtual void Render(float InAspectRatio) {}
	inline virtual void ImGui(nk_context* inContext) {}
	inline virtual void ShutDown() {}
};