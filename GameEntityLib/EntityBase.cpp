#include "./stdafx.h"
#include "GameEntityLib.h"
#include "EntityBase.h"
#include "SingletonWrapper.h"

CSingletonWrapper<IEntityManager> g_EntityManagerSingleton("GameSystem","EntityManager");
CSingletonWrapper<IPhysicManager> g_PhysicManagerSingleton("GameSystem","PhysicManager");
CSingletonWrapper<IFrameManager>  g_FrameManagerSingleton("GameSystem","FrameManager");

CEntityBase::CEntityBase()
{
    g_EntityManagerSingleton.AddRef();
    g_PhysicManagerSingleton.AddRef();
    g_FrameManagerSingleton.AddRef();

    m_dwCreationTime=g_FrameManagerSingleton.m_piInterface->GetCurrentTime();
    m_dwNextProcessFrame=0;
    m_bRemoved=false;
    m_dwDamageType=DAMAGE_TYPE_NONE;
    m_dDamage=0;
    m_dHealth=0;
    m_dMaxHealth=0;
    m_nCurrentAnimation=-1;
    g_EntityManagerSingleton.m_piInterface->AddEntity(this);
    m_piRoute=NULL;
    m_dwAlignment=ENTITY_ALIGNMENT_NEUTRAL;
    m_piTarget=NULL;
	m_dLastFrameDamage=0;
}

CEntityBase::~CEntityBase()
{
    unsigned x;
    for(x=0;x<m_dAnimations.size();x++)
    {
        IAnimation *piAnimation=m_dAnimations[x];
        delete piAnimation;
    }
    for(x=0;x<m_dWeapons.size();x++)
    {
      IWeapon *piWeapon=m_dWeapons[x];
      delete piWeapon;
    }
    g_EntityManagerSingleton.Release();
    g_PhysicManagerSingleton.Release();
    g_FrameManagerSingleton.Release();
}

void CEntityBase::OnDamage(double dDamage,IEntity *piAggresor)
{
    if(m_dwDamageType==DAMAGE_TYPE_NONE){return;}
		m_dLastFrameDamage+=dDamage;
}

void         CEntityBase::OnKilledInternal(bool bRemove){NOTIFY_EVENT(IEntityEvents,OnKilled(this));if(bRemove){Remove();}}
void         CEntityBase::OnKilled(){OnKilledInternal(true);}
bool         CEntityBase::OnCollision(IEntity *pOther,CVector &vCollisionPos){return true;}
void         CEntityBase::Remove(){m_bRemoved=true;SetCurrentAnimation(-1);NOTIFY_EVENT(IEntityEvents,OnRemoved(this));}
bool         CEntityBase::IsRemoved(){return m_bRemoved;}
void         CEntityBase::ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)
{
	if(m_dwDamageType!=DAMAGE_TYPE_NONE)
	{
		m_dHealth-=m_dLastFrameDamage;
		m_dLastFrameDamage=0;

		if(m_dHealth<=0){OnKilled();}
	}
}

SPhysicInfo *CEntityBase::GetPhysicInfo(){return &m_PhysicInfo;}
string      *CEntityBase::GetEntityClass(){return &m_sClassName;}
string      *CEntityBase::GetEntityName(){return &m_sName;}
DWORD        CEntityBase::GetNextProcessFrame(){return m_dwNextProcessFrame;}
DWORD        CEntityBase::GetAlignment(){return m_dwAlignment;}
void         CEntityBase::SetAlignment(DWORD dwAlignment){m_dwAlignment=dwAlignment;}
double       CEntityBase::GetHealth(){return m_dHealth;}
double       CEntityBase::GetMaxHealth(){return m_dMaxHealth;}

// Animaciones

void CEntityBase::AddAnimation(unsigned index,IAnimation *piAnimation)
{
    if(index<=m_dAnimations.size()){m_dAnimations.resize(index+1);}
    m_dAnimations[index]=piAnimation;
}

int  CEntityBase::GetCurrentAnimation(){return m_nCurrentAnimation;}
void CEntityBase::SetCurrentAnimation(int index)
{
    if(m_nCurrentAnimation!=-1)
    {
        IAnimation *piOldAnimation=m_dAnimations[m_nCurrentAnimation];
        if(piOldAnimation){piOldAnimation->Deactivate();}
    }
    m_nCurrentAnimation=index;
    if(m_nCurrentAnimation!=-1)
    {
      IAnimation *piAnimation=m_dAnimations[m_nCurrentAnimation];
      if(piAnimation){piAnimation->Activate(g_FrameManagerSingleton.m_piInterface->GetCurrentTime());}
    }
}

void CEntityBase::ProcessAnimations(DWORD dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished)
{
    unsigned x;
    (*pbAnimationsFinished)=true;
    for(x=0;x<m_dAnimations.size();x++)
    {
        IAnimation *piAnimation=m_dAnimations[x];
        if(piAnimation && !piAnimation->HasFinished())
        {
            (*pbAnimationsFinished)=false;
            piAnimation->ProcessFrame(g_PhysicManagerSingleton.m_piInterface,dwCurrentTime,dTimeFraction);
        }
    }
    m_dwNextProcessFrame=dwCurrentTime+1;
}

void CEntityBase::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
    unsigned x;
    for(x=0;x<m_dAnimations.size();x++)
    {
        IAnimation *piAnimation=m_dAnimations[x];
        if(piAnimation && !piAnimation->HasFinished())
        {
            piAnimation->CustomRender(piRender,piCamera);
        }
    }
}

void CEntityBase::OnAnimationEvent(string sEvent,string sParams)
{
}

void CEntityBase::SetRoute(IRoute *piRoute)
{
    m_piRoute=piRoute;
}

void CEntityBase::AddWeapon(IWeapon *piWeapon)
{
  m_dWeapons.push_back(piWeapon);
}

void CEntityBase::FireWeapon(DWORD dwWeaponSlot,DWORD dwCurrentTime)
{
  size_t x;
  for(x=0;x<m_dWeapons.size();x++)
  {
    IWeapon *piWeapon=m_dWeapons[x];
    if(piWeapon->GetSlot()==dwWeaponSlot)
    {
      piWeapon->Fire(dwCurrentTime);
    }
  }
}

IEntityManager *CEntityBase::GetEntityManager(){return g_EntityManagerSingleton.m_piInterface;}
IPhysicManager *CEntityBase::GetPhysicManager(){return g_PhysicManagerSingleton.m_piInterface;}
IFrameManager  *CEntityBase::GetFrameManager(){return g_FrameManagerSingleton.m_piInterface;}
IEntity *CEntityBase::GetTarget()
{
  return m_piTarget;
}

DWORD CEntityBase::GetDamageType()
{
  return m_dwDamageType;
}

CTraceInfo CEntityBase::GetTrace(const  CVector &p1,const CVector &p2 )
{
	CTraceInfo traceInfo;
	if(m_nCurrentAnimation!=-1)
	{
		traceInfo=m_dAnimations[m_nCurrentAnimation]->GetTrace(m_PhysicInfo.vPosition,m_PhysicInfo.vAngles,p1,p2);
	}
	else
	{
		traceInfo.m_dTraceFraction=1.0;
		traceInfo.m_vTracePos=p2;
	}
	return traceInfo;
}