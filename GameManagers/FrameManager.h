#pragma once

class CFrameManager : virtual public CSystemObjectBase, virtual public IFrameManager
{
	bool	m_bPaused;
	unsigned int m_dwPauseStartTime;

	unsigned int m_dwCurrentRealTime;
	unsigned int m_dwCurrentTime;
	unsigned int m_dwTimeBase;
	unsigned int m_dwLastTime;
	unsigned int m_dwLastRealTime;
	double m_dTimeFraction;
	double m_dRealTimeFraction;

	bool m_bSetPauseOnNextFrame;
	bool m_bContinueOnNextFrame;
	bool m_bTogglePauseOnNextFrame;

	void ExecuteTogglePause();
	
#pragma message("Encapsular los timers de alta resolucion")
#ifdef WIN32
	LARGE_INTEGER m_ldPerformanceFrequency;
#endif

	double m_dCurrentFps;
	unsigned int  m_dwFPSFrameTimes[10000];
	unsigned int  m_nFPSFrames;

	void ComputeFps();

public:

	void Reset();
	bool IsPaused();
	void SetPauseOnNextFrame(bool bPause);
	void TogglePauseOnNextFrame();

	void ProcessFrame();

	unsigned int  GetCurrentRealTime();
	unsigned int  GetCurrentTime();
	double GetTimeFraction();
	double GetRealTimeFraction();
	double GetCurrentFps();

	CFrameManager();
};