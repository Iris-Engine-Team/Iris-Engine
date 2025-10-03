#include <iostream>
#include "Core/Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	nShowCmd;
	lpCmdLine;
	hPrevInstance;
	hInstance;

	Iris::Engine::Application IEApp;

	IEApp.Initialize();
	IEApp.Run();
	IEApp.Terminate();

	return 0;
}