#include "pch.h"
#include <iostream>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")


DWORD WINAPI run_python(PVOID pParam) {
	AllocConsole();
	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONOUT$", "w+t", stderr);

	wchar_t dllPath[MAX_PATH];
	wchar_t pyDllPath[MAX_PATH];
	HMODULE dModule = GetModuleHandleW(L"pyexe.dll");
	GetModuleFileNameW(dModule, dllPath, MAX_PATH);

	PathRemoveFileSpec(dllPath);
	PathCombine(pyDllPath, dllPath, L"python310.dll");
	
	HMODULE pyModule = LoadLibraryW(pyDllPath);

	FARPROC Py_Main_addr = GetProcAddress(pyModule, "Py_Main");
	typedef int(*Py_Main_Type)(int, wchar_t**);
	Py_Main_Type Py_Main = (Py_Main_Type)Py_Main_addr;

	FARPROC PyRun_SimpleString_addr = GetProcAddress(pyModule, "PyRun_SimpleString");
	typedef void(*PyRun_SimpleString_Type)(const char *);
	PyRun_SimpleString_Type PyRun_SimpleString = (PyRun_SimpleString_Type)PyRun_SimpleString_addr;

	FARPROC Py_InitializeEx_addr = GetProcAddress(pyModule, "Py_InitializeEx");
	typedef void(*Py_InitializeEx_Type)(int);
	Py_InitializeEx_Type Py_InitializeEx = (Py_InitializeEx_Type)Py_InitializeEx_addr;

	FARPROC Py_SetProgramName_addr = GetProcAddress(pyModule, "Py_SetProgramName");
	typedef void(*Py_SetProgramName_Type)(const wchar_t *);
	Py_SetProgramName_Type Py_SetProgramName = (Py_SetProgramName_Type)Py_SetProgramName_addr;

	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);

    int argc = 1;
	wchar_t* argv[2];
	argv[0] = exePath;
	argv[1] = nullptr;

	Py_SetProgramName(exePath);
	Py_InitializeEx(1);
	//PyRun_SimpleString("with open('D:\\\\1.txt', 'w') as f: f.write('aaaaa')");
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

