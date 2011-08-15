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
#include "StdAfx.h"

template<class ITF>
class CSingletonWrapper
{
	unsigned int m_dwRefs;
	string m_sSystem;
	string m_sName;

public:

	ITF *m_piInterface;

	void AddRef()
	{
		if(m_dwRefs==0)
		{
			ISystemObject		*piObj=NULL;
			ISystemManager	*piSystemManager=GetSystemManager();
			ISystem					*piSystem=piSystemManager->GetSystem(m_sSystem);
			if(piSystem){piSystem->GetObject(m_sName,&piObj);}
			if(piObj){m_piInterface=QI(ITF,piObj);m_dwRefs++;}
			REL(piObj);
			REL(piSystem);
			REL(piSystemManager);
//			RTASSERT(m_piInterface!=NULL);
		}
		else
		{
			m_dwRefs++;
		}
	}

	void Release()
	{
		m_dwRefs--;
		if(m_dwRefs==0){REL(m_piInterface);}
	}

	CSingletonWrapper(string sSystem,string sName){m_piInterface=NULL;m_dwRefs=0;m_sSystem=sSystem;m_sName=sName;}
	~CSingletonWrapper()
	{
		m_dwRefs=0;
		REL(m_piInterface);
	}
};
