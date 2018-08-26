// Bridge.h

#pragma once

using namespace System;
#include "../TABOsu/TABOsu.h"

namespace Bridge {

	public ref class TABOsuBridge
	{
	public:
		TABOsuBridge();
		~TABOsuBridge();

		bool SetupOsuInfoWrapper();
		void StartBotWrapper();
		void StopBotWrapper();

		void SetCursorDanceMode(bool cursorDance);
		void SetAutoHitWrapper(bool autoHit);
		void SetAutoAimWrapper(bool autoAim);
		void SetAutoSpinWrapper(bool autoSpin);

		void SetAimAccuracyWrapper(double aimAccuracy);
		void SetHitAccuracyWrapper(double hitAccuracy);

		bool LoadSongWrapper(String^ path);

	private:
		CTABOsu* m_pCTABOsu;
	};
}
