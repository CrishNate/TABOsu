// TABOsu.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TABOsu.h"
#include <fstream>
#include <tlhelp32.h>

// This is an example of an exported variable
TABOSU_API int nTABOsu=0;


// This is an example of an exported function.
TABOSU_API int fnTABOsu(void)
{
	return 42;
}


// This is the constructor of a class that has been exported.
// see TABOsu.h for the class definition
CTABOsu::CTABOsu()
	: m_pThreadReadStack(new CThReadStack())
	, m_pOsuHander(new OsuHandler())
	, m_HWND(NULL)
{
	SetupOsuInfo();
}

DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

bool CTABOsu::SetupOsuInfo()
{
	if (!m_HWND)
	{
		DWORD dwProcID = FindProcessId(L"osu!.exe");
		DWORD thReadStackBase = NULL;

		m_HWND = FindMainWindow(dwProcID);
		m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);
		thReadStackBase = m_pThreadReadStack->GetThReadStack(m_hProcess, dwProcID);
		ReadProcessMemory(m_hProcess, (LPCVOID)(thReadStackBase - 0x32C), &m_dwReadStackBase, sizeof(DWORD), NULL);

		if (dwProcID)
			return true;

		return false;
	}
}

void CTABOsu::StartBot()
{
	m_pOsuHander->StartBot(m_HWND, m_hProcess, m_dwReadStackBase);
}

void CTABOsu::StopBot()
{
	m_pOsuHander->StopBot();
}


void CTABOsu::SetOsuPath(const std::wstring& path)
{
	m_pOsuHander->SetOsuPath(path);
}


bool CTABOsu::LoadSong(const std::wstring& path)
{
	return m_pOsuHander->LoadSong(path);
}


void CTABOsu::SetAutoHit(bool autoHit)
{
	m_pOsuHander->SetAutoHit(autoHit);
}


void CTABOsu::SetAutoAim(bool autoAim)
{
	m_pOsuHander->SetAutoAim(autoAim);
}


void CTABOsu::SetAutoSpin(bool autoSpin)
{
	m_pOsuHander->SetAutoSpin(autoSpin);
}