// This is the main DLL file.

#include "stdafx.h"
#include "Bridge.h"

#include <msclr\marshal_cppstd.h>

namespace Bridge {

	TABOsuBridge::TABOsuBridge()
	{
		m_pCTABOsu = new CTABOsu();
	}

	TABOsuBridge::~TABOsuBridge()
	{

	}

	bool TABOsuBridge::SetupOsuInfoWrapper()
	{
		return m_pCTABOsu->SetupOsuInfo();
	}

	void TABOsuBridge::StartBotWrapper()
	{
		m_pCTABOsu->StartBot();
	}

	void TABOsuBridge::StopBotWrapper()
	{
		m_pCTABOsu->StopBot();
	}


	void TABOsuBridge::SetOsuPathWrapper(String^ path)
	{
		std::wstring nativePath
			= msclr::interop::marshal_as<std::wstring>(path);
		m_pCTABOsu->SetOsuPath(nativePath);
	}


	bool TABOsuBridge::LoadSongWrapper(String^ path)
	{
		std::wstring nativePath
			= msclr::interop::marshal_as<std::wstring>(path);

		return m_pCTABOsu->LoadSong(nativePath);
	}


	void TABOsuBridge::SetAutoHitWrapper(bool autoHit)
	{
		m_pCTABOsu->SetAutoHit(autoHit);
	}


	void TABOsuBridge::SetAutoAimWrapper(bool autoAim)
	{
		m_pCTABOsu->SetAutoAim(autoAim);
	}


	void TABOsuBridge::SetAutoSpinWrapper(bool autoSpin)
	{
		m_pCTABOsu->SetAutoSpin(autoSpin);
	}
}