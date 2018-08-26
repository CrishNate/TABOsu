// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TABOSU_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TABOSU_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include "stdafx.h"
#include <string>
#include <memory>
#include "OsuHandler.h"

#ifdef TABOSU_EXPORTS
#define TABOSU_API __declspec(dllexport)
#else
#define TABOSU_API __declspec(dllimport)
#endif


class CThReadStack;

// This class is exported from the TABOsu.dll
class TABOSU_API CTABOsu {
public:
	CTABOsu(void);

	bool SetupOsuInfo();
	void StartBot();
	void StopBot();

	void SetCursorDance(bool cursorDance);
	void SetAutoHit(bool autoHit);
	void SetAutoAim(bool autoAim);
	void SetAutoSpin(bool autoSpin);
	void SetAimAccuracy(float aimAccuracy);
	void SetHitAccuracy(float hitAccuracy);

	bool LoadSong(const std::wstring& path);

private:

	CThReadStack* m_pThreadReadStack;
	OsuHandler* m_pOsuHander;

	DWORD m_dwReadStackBase;
	HWND m_HWND;
	HANDLE m_hProcess;
	RECT m_WindowRect;
};

extern TABOSU_API int nTABOsu;

TABOSU_API int fnTABOsu(void);
