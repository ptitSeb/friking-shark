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


struct STimeMeterFrameData
{
	double dStartTime;
	double dEndTime;
	std::map<std::string,double> m_mSteps;
	
	STimeMeterFrameData();
};

class CTimeMeter
{
	// microseconds
	bool 		 m_bActive;
	unsigned int m_nLastDumpTime;
	unsigned int m_nFramesToTake;
	double 		 m_dCurrentStepStart;
	double		 m_dLastFrameEnd;
	double		 m_dExternalTime;
	
	std::deque<STimeMeterFrameData*> m_vFrames;
	STimeMeterFrameData *m_pCurrentFrame;
	
	std::string m_sCurrentStepName;
	
public:
	void SetActive(bool bActive);
	bool IsActive();
	
	void Start();
	void SetStep(const char*sStep);
	void EndStep();
	void End();
	void Dump(unsigned int nInterval=1000);
	
	CTimeMeter(unsigned int nFramesToTake=100);
	~CTimeMeter();
};

#define RTTIMEMETER_SETSTEP(name)    {GetTimeMeter()->SetStep(name);}
#define RTTIMEMETER_ENDSTEP()        {GetTimeMeter()->EndStep();}
