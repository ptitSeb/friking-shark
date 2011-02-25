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
