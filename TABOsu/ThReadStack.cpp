#include "stdafx.h"
#include "ThReadStack.h"
#include <tlhelp32.h>
#include <Psapi.h>
#include "ntinfo.h"
#include <vector>

std::vector<DWORD> threadList(DWORD pid);
DWORD GetThreadStartAddress(HANDLE processHandle, HANDLE hThread);

CThReadStack::CThReadStack() {
}

DWORD CThReadStack::GetThReadStack(HANDLE hProcese, DWORD dwProcID){
	DWORD dwThRS;

	std::vector<DWORD> threadId = threadList(dwProcID);
	int stackNum = 0;

	HANDLE threadHandle = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, *threadId.begin());
	dwThRS = GetThreadStartAddress(hProcese, threadHandle);

	return dwThRS;
}

std::vector<DWORD> threadList(DWORD pid) {
	/* solution from http://stackoverflow.com/questions/1206878/enumerating-threads-in-windows */
	std::vector<DWORD> vect = std::vector<DWORD>();
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h == INVALID_HANDLE_VALUE)
		return vect;

	THREADENTRY32 te;
	te.dwSize = sizeof(te);
	if (Thread32First(h, &te)) {
		do {
			if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
				sizeof(te.th32OwnerProcessID)) {

				if (te.th32OwnerProcessID == pid) {
					printf("PID: %04d Thread ID: 0x%04x\n", te.th32OwnerProcessID, te.th32ThreadID);
					vect.push_back(te.th32ThreadID);
				}
			}
			te.dwSize = sizeof(te);
		} while (Thread32Next(h, &te));
	}

	return vect;
}

DWORD GetThreadStartAddress(HANDLE processHandle, HANDLE hThread) {
	/* rewritten from https://github.com/cheat-engine/cheat-engine/blob/master/Cheat%20Engine/CEFuncProc.pas#L3080 */
	DWORD used = 0, ret = 0;
	DWORD stacktop = 0, result = 0;

	MODULEINFO mi;

	GetModuleInformation(processHandle, GetModuleHandle(L"kernel32.dll"), &mi, sizeof(mi));
	stacktop = (DWORD)GetThreadStackTopAddress_x86(processHandle, hThread);

	CloseHandle(hThread);

	if (stacktop) {
		//find the stack entry pointing to the function that calls "ExitXXXXXThread"
		//Fun thing to note: It's the first entry that points to a address in kernel32

		DWORD* buf32 = new DWORD[4096];

		if (ReadProcessMemory(processHandle, (LPCVOID)(stacktop - 4096), buf32, 4096, NULL)) {
			for (int i = 4096 / 4 - 1; i >= 0; --i) {
				if (buf32[i] >= (DWORD)mi.lpBaseOfDll && buf32[i] <= (DWORD)mi.lpBaseOfDll + mi.SizeOfImage) {
					result = stacktop - 4096 + i * 4;
					break;
				}
			}
		}

		delete buf32;
	}

	return result;
}


BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}


BOOL CALLBACK EnumWindowsCallbackE(HWND handle, LPARAM lParam)
{
	HandleData& data = *(HandleData*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !IsMainWindow(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
}


HWND FindMainWindow(DWORD process_id)
{
	HandleData data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(EnumWindowsCallbackE, (LPARAM)&data);
	return data.best_handle;
}
