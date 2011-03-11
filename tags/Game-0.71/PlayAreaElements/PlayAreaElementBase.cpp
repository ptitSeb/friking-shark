#include "./stdafx.h"
#include "PlayAreaElementBase.h"

CPlayAreaElementBase::CPlayAreaElementBase()
{
    m_bActive=false;
    m_dwActivationTime=0;
}

CPlayAreaElementBase::~CPlayAreaElementBase()
{
}

void CPlayAreaElementBase::Activate(unsigned int dwCurrentTime){m_bActive=true;m_dwActivationTime=dwCurrentTime;}
void CPlayAreaElementBase::Deactivate(){m_bActive=false;m_dwActivationTime=0;}
bool CPlayAreaElementBase::IsActive(){return m_bActive;}
void CPlayAreaElementBase::Reset(){Deactivate();}
bool CPlayAreaElementBase::ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pPlayAreaInfo,unsigned int dwCurrentTime,double dInterval){return true;}
bool Util_IsInPlayArea(CVector vPosition,SPlayAreaInfo *pInfo)
{
    for(int x=0;x<pInfo->nPlaneCount;x++)
    {
        if(pInfo->planes[x].GetSide(vPosition)>0){return false;}
    }
    return true;
}
