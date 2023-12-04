#include "pch.h"
#include <iostream>
#include <filesystem>
#include <regex>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")


std::wstring findPythonDLL(const std::wstring& path) {
	std::wregex pattern(L"python3[0-9]+\\.dll");

	for (const auto & entry : std::filesystem::directory_iterator(path)) {
		std::wstring fileName = entry.path().filename().wstring();
		if (std::regex_match(fileName, pattern)) {
			return fileName;
		}
	}

	return L"";
}


DWORD WINAPI run_python(LPVOID lpParam) {
	AllocConsole();
	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONOUT$", "w+t", stderr);

	wchar_t dllPath[MAX_PATH];
	wchar_t pyDllPath[MAX_PATH];
	HMODULE dModule = static_cast<HMODULE>(lpParam);
	GetModuleFileNameW(dModule, dllPath, MAX_PATH);

	PathRemoveFileSpec(dllPath);
	std::wstring pyDllName = findPythonDLL(dllPath);
	PathCombine(pyDllPath, dllPath, pyDllName.c_str());
	
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

	FARPROC Py_SetPythonHome_addr = GetProcAddress(pyModule, "Py_SetPythonHome");
	typedef void(*Py_SetPythonHome_Type)(const wchar_t *);
	Py_SetPythonHome_Type Py_SetPythonHome = (Py_SetPythonHome_Type)Py_SetPythonHome_addr;

	FARPROC PyImport_ImportModule_addr = GetProcAddress(pyModule, "PyImport_ImportModule");
	typedef void*(*PyImport_ImportModule_Type)(const char *);
	PyImport_ImportModule_Type PyImport_ImportModule = (PyImport_ImportModule_Type)PyImport_ImportModule_addr;

	FARPROC PyObject_GetAttrString_addr = GetProcAddress(pyModule, "PyObject_GetAttrString");
	typedef void *(*PyObject_GetAttrString_Type)(void *, const char *);
	PyObject_GetAttrString_Type PyObject_GetAttrString = (PyObject_GetAttrString_Type)PyObject_GetAttrString_addr;

	FARPROC PyUnicode_FromWideChar_addr = GetProcAddress(pyModule, "PyUnicode_FromWideChar");
	typedef void *(*PyUnicode_FromWideChar_Type)(const wchar_t *, size_t);
	PyUnicode_FromWideChar_Type PyUnicode_FromWideChar = (PyUnicode_FromWideChar_Type)PyUnicode_FromWideChar_addr;

	FARPROC PyList_Insert_addr = GetProcAddress(pyModule, "PyList_Insert");
	typedef int(*PyList_Insert_Type)(void *, size_t, void *);
	PyList_Insert_Type PyList_Insert = (PyList_Insert_Type)PyList_Insert_addr;

	wchar_t exePath[MAX_PATH];
	GetModuleFileNameW(NULL, exePath, MAX_PATH);

    int argc = 1;
	wchar_t* argv[2];
	argv[0] = exePath;
	argv[1] = nullptr;

	Py_SetPythonHome(dllPath);
	Py_SetProgramName(exePath);
	Py_InitializeEx(1);
	PyRun_SimpleString("print('load python success!')");
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
		CreateThread(NULL, 0, run_python, hModule, 0, NULL);
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

