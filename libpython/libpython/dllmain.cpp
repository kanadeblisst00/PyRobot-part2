#include "pch.h"
#include "Python.h"
#include <iostream>


DWORD WINAPI run_python(PVOID pParam) {
	AllocConsole();
	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONOUT$", "w+t", stderr);

	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);

	int argc = 1;
	wchar_t* argv[2];
	argv[0] = exePath;
	argv[1] = nullptr;

	Py_SetProgramName(exePath);
	Py_InitializeEx(1);
	
	PyRun_SimpleString("print('hello')");

	Py_Main(argc, argv);
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH: {
		CreateThread(NULL, 0, run_python, NULL, 0, NULL);
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

