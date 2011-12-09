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


#include "./stdafx.h"
#include "TimeMeter.h"


STimeMeterFrameData::STimeMeterFrameData()
{
	dStartTime=dEndTime=0.0;
}

CTimeMeter::CTimeMeter(unsigned int nFramesToTake)
{
	m_bActive=false;
	m_dExternalTime=0;
	m_dLastFrameEnd=0;
	m_nLastDumpTime=0;
	m_dCurrentStepStart=0;
	m_nFramesToTake=nFramesToTake;
	m_pCurrentFrame=NULL;
}

CTimeMeter::~CTimeMeter()
{	
}

void CTimeMeter::SetActive(bool bActive)
{
	m_bActive=bActive;
}

bool CTimeMeter::IsActive()
{
	return m_bActive;
}

void CTimeMeter::Start()
{
	if(!m_bActive){return;}
	if(m_vFrames.size()>=m_nFramesToTake)
	{
		STimeMeterFrameData *pOld=m_vFrames.front();
		m_vFrames.pop_front();
		delete pOld;
	}
	
	m_dCurrentStepStart=GetMicrosecondTimeStamp();
	m_sCurrentStepName="";
	if(m_dLastFrameEnd)
	{
		m_dExternalTime=m_dCurrentStepStart-m_dLastFrameEnd;
	}
	
	STimeMeterFrameData *pData=new STimeMeterFrameData;
	pData->dStartTime=m_dCurrentStepStart;
	m_vFrames.push_back(pData);
	m_pCurrentFrame=pData;
}

void CTimeMeter::SetStep(const char*sStep)
{
	if(!m_bActive){return;}
	if(m_pCurrentFrame==NULL){return;}

	double dCurrentTime=GetMicrosecondTimeStamp();
	if(m_sCurrentStepName.length())
	{
		m_pCurrentFrame->m_mSteps[m_sCurrentStepName]+=dCurrentTime-m_dCurrentStepStart;
	}

	m_dCurrentStepStart=dCurrentTime;
	m_sCurrentStepName=sStep;
}

void CTimeMeter::EndStep()
{
	if(!m_bActive){return;}
	if(m_pCurrentFrame==NULL){return;}
	if(m_sCurrentStepName.length()==0){return;}
	
	double dCurrentTime=GetMicrosecondTimeStamp();
	m_pCurrentFrame->m_mSteps[m_sCurrentStepName]+=dCurrentTime-m_dCurrentStepStart;
	m_dCurrentStepStart=0;
	m_sCurrentStepName="";
}

void CTimeMeter::End()
{
	if(!m_bActive){return;}
	if(m_pCurrentFrame==NULL){return;}
	
	double dCurrentTime=GetMicrosecondTimeStamp();
	if(m_sCurrentStepName.length())
	{
		m_pCurrentFrame->m_mSteps[m_sCurrentStepName]+=dCurrentTime-m_dCurrentStepStart;
	}
	m_pCurrentFrame->dEndTime=dCurrentTime;
	m_pCurrentFrame=NULL;
	m_dLastFrameEnd=dCurrentTime;
	m_dCurrentStepStart=0;
	m_sCurrentStepName="";
}

void CTimeMeter::Dump(unsigned int nInterval)
{
	if(!m_bActive){return;}
	if(!m_vFrames.size()){return;}
	
	unsigned int nCurrentTimeMS=GetTimeStamp();
	if(m_nLastDumpTime+nInterval>nCurrentTimeMS){return;}
	m_nLastDumpTime=nCurrentTimeMS;
	
	
	double dTotalFrameTime=0.0;
	double dTotalAccounted=0.0;
	double dFrameCount=m_vFrames.size();
	unsigned int x=0;
	
	std::map<std::string,double> mSteps;
	std::map<std::string,double>::iterator i;
	
	for(x=0;x<m_vFrames.size();x++)
	{
		dTotalFrameTime+=(m_vFrames[x]->dEndTime-m_vFrames[x]->dStartTime);
		
		for(i=m_vFrames[x]->m_mSteps.begin();i!=m_vFrames[x]->m_mSteps.end();i++)
		{
			mSteps[i->first]+=i->second;
			dTotalAccounted+=i->second;
		}
	}
	STimeMeterFrameData *pLastFrame=m_vFrames[m_vFrames.size()-1];
	
	double dAvgFrameTimeMS=(dTotalFrameTime/dFrameCount)/1000.0;
	double dAvgExternalTimeMS=(m_dExternalTime/dFrameCount)/1000.0;
	double dAvgAccountedMS=(dTotalAccounted/dFrameCount)/1000.0;
	double dPercentAccounted=(dTotalAccounted*100.0/dTotalFrameTime);
	RTTRACE("---- %d Frames ----",(int)dFrameCount);
	RTTRACE("Avg.External time %f",dAvgExternalTimeMS);
	RTTRACE("Avg.Round time    %f, fps %f",dAvgFrameTimeMS+dAvgExternalTimeMS,1000.0/(dAvgFrameTimeMS+dAvgExternalTimeMS),dAvgExternalTimeMS);
	RTTRACE("Avg.Frame time    %f, fps %f",dAvgFrameTimeMS,1000.0/dAvgFrameTimeMS);
	RTTRACE("Avg.Accounted     %f, %.f%%",dAvgAccountedMS,dPercentAccounted);
	for(i=mSteps.begin();i!=mSteps.end();i++)
	{
		double dStepMS=(i->second/dFrameCount)/1000.0;
		RTTRACE("%-30s: %f ms (%2.f%%)",i->first.c_str(),dStepMS,dStepMS*100.0/dAvgFrameTimeMS);
	}
}