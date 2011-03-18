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
	m_nCurrentState=ENTITY_STATE_INVALID;
	m_nCurrentStateAnimation=ANIMATION_INVALID;
	
    g_EntityManagerSingleton.m_piInterface->AddEntity(this);
    m_piRoute=NULL;
    m_dwAlignment=ENTITY_ALIGNMENT_NEUTRAL;
    m_piTarget=NULL;
	m_dLastFrameDamage=0;
}

CEntityBase::~CEntityBase()
{
    unsigned x;
    for(x=0;x<m_vActiveAnimations.size();x++)
    {
        IAnimation *piAnimation=m_vActiveAnimations[x];
        delete piAnimation;
    }
    for(x=0;x<m_vWeapons.size();x++)
    {
      IWeapon *piWeapon=m_vWeapons[x];
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
void         CEntityBase::Remove(){m_bRemoved=true;SetState(ENTITY_STATE_INVALID,ANIMATION_INVALID);NOTIFY_EVENT(IEntityEvents,OnRemoved(this));}
bool         CEntityBase::IsRemoved(){return m_bRemoved;}
void         CEntityBase::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
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
unsigned int        CEntityBase::GetNextProcessFrame(){return m_dwNextProcessFrame;}
unsigned int        CEntityBase::GetAlignment(){return m_dwAlignment;}
void         CEntityBase::SetAlignment(unsigned int dwAlignment){m_dwAlignment=dwAlignment;}
double       CEntityBase::GetHealth(){return m_dHealth;}
double       CEntityBase::GetMaxHealth(){return m_dMaxHealth;}
void		 CEntityBase::SetHealth(double dHealth){m_dHealth=dHealth;}
void		 CEntityBase::SetMaxHealth(double dMaxHealth){m_dMaxHealth=dMaxHealth;}
// Animaciones

unsigned int  CEntityBase::GetState(){return m_nCurrentState;}

void CEntityBase::SetState(unsigned int nState,unsigned int nAnimation)
{
	if(nState==m_nCurrentState && nAnimation==ANIMATION_RANDOM){return;}
	if(nState==m_nCurrentState && m_nCurrentStateAnimation==nAnimation){return;}
	
	m_nCurrentState=nState;
	m_nCurrentStateAnimation=(nState==ENTITY_STATE_INVALID)?ANIMATION_INVALID:nAnimation;
	
	for(unsigned int x=0;x<m_vActiveAnimations.size();x++)
	{
		IAnimation *piAnimation=m_vActiveAnimations[x];
		if(piAnimation && !piAnimation->HasFinished())
		{
			piAnimation->Deactivate();
		}
	}
	if(nState!=ENTITY_STATE_INVALID)
	{
		unsigned long nTime=g_FrameManagerSingleton.m_piInterface->GetCurrentTime();
		IAnimation *piAnimation=m_pTypeBase->CreateStateAnimation(this,nState,nAnimation,nTime);
		if(piAnimation)
		{
			piAnimation->Activate(nTime);
			m_vActiveAnimations.push_back(piAnimation);
		}
	}
}

void CEntityBase::ProcessAnimations(unsigned int dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished)
{
    (*pbAnimationsFinished)=true;
	std::vector<IAnimation*>::iterator i;
	for(i=m_vActiveAnimations.begin();i!=m_vActiveAnimations.end();)
    {
        IAnimation *piAnimation=*i;
        if(piAnimation->HasFinished())
		{
			i=m_vActiveAnimations.erase(i);
			delete piAnimation;
		}
		else
        {
            (*pbAnimationsFinished)=false;
            piAnimation->ProcessFrame(g_PhysicManagerSingleton.m_piInterface,dwCurrentTime,dTimeFraction);
			i++;
        }
    }
    m_dwNextProcessFrame=dwCurrentTime+1;
}

void CEntityBase::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
    unsigned x;
	for(x=0;x<m_vActiveAnimations.size();x++)
    {
		IAnimation *piAnimation=m_vActiveAnimations[x];
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
  m_vWeapons.push_back(piWeapon);
}

void CEntityBase::FireWeapon(unsigned int dwWeaponSlot,unsigned int dwCurrentTime)
{
  size_t x;
  for(x=0;x<m_vWeapons.size();x++)
  {
    IWeapon *piWeapon=m_vWeapons[x];
    if(piWeapon->GetSlot()==dwWeaponSlot)
    {
      piWeapon->Fire(dwCurrentTime);
    }
  }
}

IEntityManager *CEntityBase::GetEntityManager(){return g_EntityManagerSingleton.m_piInterface;}
IEntity *CEntityBase::GetTarget()
{
  return m_piTarget;
}

unsigned int CEntityBase::GetDamageType(){return m_dwDamageType;}
void 		 CEntityBase::SetDamageType(unsigned int dwDamageType){m_dwDamageType=dwDamageType;}

CTraceInfo CEntityBase::GetTrace(const  CVector &p1,const CVector &p2 )
{
	CTraceInfo traceInfo;
	traceInfo.m_dTraceFraction=1.0;
	traceInfo.m_vTracePos=p2;
	
	for(unsigned int x=0;x<m_vActiveAnimations.size();x++)
	{
		IAnimation *piAnimation=m_vActiveAnimations[x];
		if(piAnimation && !piAnimation->HasFinished())
		{
			CTraceInfo tempTraceInfo;
			tempTraceInfo=piAnimation->GetTrace(m_PhysicInfo.vPosition,m_PhysicInfo.vAngles,p1,p2);
			if(tempTraceInfo.m_bTraceHit && tempTraceInfo.m_dTraceFraction<traceInfo.m_dTraceFraction)
			{
				traceInfo=tempTraceInfo;
			}
		}
	}	
	return traceInfo;
}

void CEntityBase::SetEntityTypeBase(CEntityTypeBase *pTypeBase)
{
	m_pTypeBase=pTypeBase;
}
