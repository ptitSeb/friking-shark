#include "StdAfx.h"
#include "GameRunTimeLib.h"
#include "InterfaceLeakAPI.h"
#include ".\systemunknownbase.h"

CSystemUnknownBase::CSystemUnknownBase(void)
{
	//RegisterAddRef(this);
   m_dwReferences=1;
}

CSystemUnknownBase::~CSystemUnknownBase(void)
{
}

void CSystemUnknownBase::AddReference()
{
	//RegisterAddRef(this);
    m_dwReferences++;
}

void CSystemUnknownBase::FinalRelease()
{
}

void CSystemUnknownBase::ReleaseReference()
{
	//RegisterRelease(this);
    CSystemUnknownBase *pThis=this;
    m_dwReferences--;
    if(pThis->m_dwReferences==0)
		{
			FinalRelease();
			delete pThis;
		}
}