#pragma once

class CFrameManager : virtual public CSystemObjectBase, virtual public IFrameManager
{
	bool	m_bPaused;
	DWORD m_dwPauseStartTime;

	DWORD m_dwCurrentRealTime;
	DWORD m_dwCurrentTime;
	DWORD m_dwTimeBase;
	DWORD m_dwLastTime;
	DWORD m_dwLastRealTime;
	double m_dTimeFraction;
	double m_dRealTimeFraction;

	bool m_bSetPauseOnNextFrame;
	bool m_bContinueOnNextFrame;
	bool m_bTogglePauseOnNextFrame;

	void ExecuteTogglePause();
	
	LARGE_INTEGER m_ldPerformanceFrequency;

	double m_dCurrentFps;
	DWORD  m_dwFPSFrameTimes[10000];
	DWORD  m_nFPSFrames;

	void ComputeFps();

public:

	void Reset();
	bool IsPaused();
	void SetPauseOnNextFrame(bool bPause);
	void TogglePauseOnNextFrame();

	void ProcessFrame();

	DWORD  GetCurrentRealTime();
	DWORD  GetCurrentTime();
	double GetTimeFraction();
	double GetRealTimeFraction();
	double GetCurrentFps();

	CFrameManager();
};