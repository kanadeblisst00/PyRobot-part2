import os
import time
import platform
from winapi import *


def get_first_dll():
    dllpath = None
    for file in os.listdir('.'):
        if file.endswith('.dll'):
            dllpath = file
            break
    return dllpath

def inject_dll(pid, dllpath=None):
    '''注入dll到给定的进程，返回http端口'''
    dllpath = dllpath or get_first_dll()
    if not dllpath:
        raise Exception("给定的dllpath不存在")
    dllpath = os.path.abspath(dllpath)
    if not os.path.exists(dllpath):
        raise Exception('给定的dllpath不存在')
    dllname = os.path.basename(dllpath)
    dll_addr = getModuleBaseAddress(dllname, pid)
    if dll_addr:
        print("当前进程已存在相同名称的dll")
        return dll_addr
    # 通过微信进程pid获取进程句柄
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, False, pid)
    # 在微信进程中申请一块内存
    lpAddress = VirtualAllocEx(hProcess, None, MAX_PATH, MEM_COMMIT, PAGE_EXECUTE_READWRITE)
    # 往内存中写入要注入的dll的绝对路径
    WriteProcessMemory(hProcess, lpAddress, c_wchar_p(dllpath), MAX_PATH, byref(c_ulong()))
    # 因为注入的微信是32位的，所以运行的Python也需要是32位的
    if platform.architecture()[0] != '32bit':
        raise Exception("需要使用32位Python才能正常执行，请更换后重试!")
    # 在微信进程内调用LoadLibraryW加载dll
    hRemote = CreateRemoteThread(hProcess, None, 0, LoadLibraryW, lpAddress, 0, None)
    time.sleep(0.01)
    # 关闭句柄
    CloseHandle(hProcess)
    CloseHandle(hRemote)
    time.sleep(0.01)
    dll_addr = getModuleBaseAddress(dllname, pid)
    return dll_addr


def uninject_dll(pid, dllname):
    dll_addr = getModuleBaseAddress(dllname, pid)
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, False, pid)
    while dll_addr:
        hRemote = CreateRemoteThread(hProcess, None, 0, FreeLibrary, dll_addr, 0, None)
        CloseHandle(hRemote)
        time.sleep(0.01)
        dll_addr = getModuleBaseAddress(dllname, pid)
    CloseHandle(hProcess)
    

if __name__ == "__main__":
    pids = enumProcess("CtypesTest.exe")
    print("pids: ", pids)
    dllpath = r"T:\Code\compile_python\python-3.10.11-embed-win32\pyexe.dll"
    # dllpath = r"T:\Code\compile_python\libpython\Debug\libpython.dll"
    addr = inject_dll(pids[0], dllpath)
    print("addr: ", addr)
    #uninject_dll(pids[0], "python310.dll")
    
    