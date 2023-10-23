#include <windows.h>
#include <iostream>


int wmain(int argc, wchar_t* argv[]) {
	// 指定要加载的DLL，这里我打算放到同一目录，就不写绝对路径了
	const wchar_t* dllFileName = L"python310.dll";
	// 加载DLL
	HMODULE hModule = LoadLibraryW(dllFileName);
	// 获取导出函数地址
	FARPROC Py_Main_addr = GetProcAddress(hModule, "Py_Main"); // 替换为导出函数的名称
	// 定义函数指针和调用函数
	typedef int(*Py_Main_Type)(int, wchar_t**);
	Py_Main_Type Py_Main = (Py_Main_Type)Py_Main_addr;

	int result = Py_Main(argc, argv);

	return 0;
}
