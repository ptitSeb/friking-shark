//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


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
	
	double        m_dCurrentFps;
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