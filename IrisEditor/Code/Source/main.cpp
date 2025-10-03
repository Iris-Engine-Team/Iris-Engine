#include <iostream>
#include "Core/IrisEditor.h"

#ifdef NDEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	nShowCmd;
	lpCmdLine;
	hPrevInstance;
	hInstance;

	Iris::Editor::IrisEditor IEEditor;

	IEEditor.Initialize();
	IEEditor.Run();
	IEEditor.Terminate();

	return 0;
}
#else
int main()
{
	//Checks memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(3428400);

	Iris::Editor::IrisEditor IEEditor;

	IEEditor.Initialize();
	IEEditor.Run();
	IEEditor.Terminate();

	return 0;
}
#endif