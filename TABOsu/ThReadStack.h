#pragma once
#include "stdafx.h"

class CThReadStack
{
public:
	CThReadStack();
	DWORD GetThReadStack(HANDLE, DWORD);
};

struct HandleData 
{
	unsigned long process_id;
	HWND best_handle;
};


BOOL IsMainWindow(HWND handle);

BOOL CALLBACK EnumWindowsCallbackE(HWND handle, LPARAM lParam);

HWND FindMainWindow(DWORD process_id);