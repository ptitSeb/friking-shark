#include "StdAfx.h"
#include ".\particlesystems.h"
#include ".\particlesystemtype.h"

CParticleSystemType::CParticleSystemType(void){}
CParticleSystemType::~CParticleSystemType(void){}

IParticleSystem *CParticleSystemType::CreateInstance(DWORD dwCurrentTime)
{
    unsigned x=0;

    CParticleSystem *pSystem=new CParticleSystem(this);
    for(x=0;x<m_dEmitters.size();x++)
    {
        IParticleEmitter *piEmitter=m_dEmitters[x].m_piEmitterType->CreateInstance(dwCurrentTime);
        if(piEmitter){pSystem->AddEmitter(piEmitter);}
    }
    for(x=0;x<m_dModifiers.size();x++)
    {
        IParticleModifierType *piModifierType=m_dModifiers[x].m_piModifierType;
        IParticleModifier     *piModifier=m_dModifiers[x].m_piModifierType->CreateInstance(dwCurrentTime);
        if(piModifier)
        {
            pSystem->AddModifier(piModifier);

            for(unsigned y=0;y<piModifierType->GetEmitterNameCount();y++)
            {
                string sEmitterName=piModifierType->GetEmitterName(y);
                IParticleEmitter *piEmitter=pSystem->GetEmitter(sEmitterName);
                if(piEmitter){piModifier->AddEmitter(piEmitter);}
            }
        }
    }

    return pSystem;
}

CParticleSystem::CParticleSystem(CParticleSystemType *pType)
{
    m_bAnglesDirty=true;
    m_pType=pType;
}
CParticleSystem::~CParticleSystem()
{
    unsigned x;
    list<IParticle*>::iterator i;
    for(i=m_lParticles.begin();i!=m_lParticles.end();i++){delete *i;}
    for(x=0;x<m_dEmitters.size();x++){delete m_dEmitters[x];}
    for(x=0;x<m_dModifiers.size();x++){delete m_dModifiers[x];}
}

void CParticleSystem::AddEmitter(IParticleEmitter *piParticleEmitter){m_dEmitters.push_back(piParticleEmitter);}
void CParticleSystem::AddModifier(IParticleModifier *piParticleModifier){m_dModifiers.push_back(piParticleModifier);}
void CParticleSystem::AddParticle(IParticle *pParticle){m_lParticles.push_back(pParticle);}

IParticleEmitter *CParticleSystem::GetEmitter(string sName)
{
    for(unsigned x=0;x<m_dEmitters.size();x++)
    {
        if(m_dEmitters[x]->GetName()==sName){return m_dEmitters[x];}
    }
    return NULL;
}

void CParticleSystem::DeactivateAllEmitters()
{
  // Si se desactivan todos los emisores tambien hay que desactivar las particulas sin tiempo de vida
  // es decir con tiempo de vida=0, ya que de otra forma las particulas nunca terminan.

  list<IParticle*>::iterator i;
  for(i=m_lParticles.begin();i!=m_lParticles.end();)
  {
    IParticle *pParticle=*i;
    if(pParticle->m_dwEndTime==0)
    {
      pParticle->m_bActive=false;
    }
    if(!pParticle->m_bActive)
    {   
      delete pParticle;
      i=m_lParticles.erase(i);
    }
    else
    {
      i++;
    }
  }
  for(unsigned x=0;x<m_dEmitters.size();x++)
  {
    m_dEmitters[x]->Deactivate();
  }
}

bool CParticleSystem::ProcessFrame(IPhysicManager *piPhysicManager,DWORD dwCurrentTime,double dInterval)
{
    bool bAllEmittersInactive=true;
    for(unsigned x=0;x<m_dEmitters.size();x++)
    {
        m_dEmitters[x]->ProcessFrame(this,dwCurrentTime,dInterval);

        if(m_dEmitters[x]->IsActive())
        {
            bAllEmittersInactive=false;
        }
    }
    list<IParticle*>::iterator i;
    for(i=m_lParticles.begin();i!=m_lParticles.end();)
    {
        IParticle *pParticle=*i;
        if(pParticle->m_dwEndTime)
        {
            pParticle->m_dLifeSpent=((double)dwCurrentTime-(double)pParticle->m_dwStartTime)/((double)pParticle->m_dwEndTime-(double)pParticle->m_dwStartTime);
        }
        if(pParticle->m_dLifeSpent>=1.0)
        {
            pParticle->m_dLifeSpent=1.0;
            pParticle->m_bActive=false;
        }
        if(!pParticle->m_bActive)
        {   
            delete pParticle;
            i=m_lParticles.erase(i);
        }
        else
        {
            if(pParticle->m_bFixedPositionOnParent)
            {
              CVector vForward,vRight,vUp;
              GetVectors(vForward,vRight,vUp);
              pParticle->m_PhysicInfo.vPosition=GetPosition();
              pParticle->m_PhysicInfo.vPosition+=vForward*pParticle->m_vPositionOnParent.c[0];
              pParticle->m_PhysicInfo.vPosition+=vUp*pParticle->m_vPositionOnParent.c[1];
              pParticle->m_PhysicInfo.vPosition+=vRight*pParticle->m_vPositionOnParent.c[2];
            }
            else
            {
              pParticle->m_PhysicInfo.vPosition=piPhysicManager->ProcessPhysicInfo(&pParticle->m_PhysicInfo,dInterval);
            }

            for(unsigned x=0;x<m_dModifiers.size();x++)
            {
                m_dModifiers[x]->ProcessParticle(pParticle,this,dwCurrentTime,dInterval);
            }
            i++;
        }
    }
    return !bAllEmittersInactive || m_lParticles.size()!=0;
}

void CParticleSystem::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
    list<IParticle *>::iterator i;
    for(i=m_lParticles.begin();i!=m_lParticles.end();i++)
    {
        IParticle *pParticle = *i;
        pParticle->CustomRender(piRender,piCamera);    
    }
}

CVector CParticleSystem::GetPosition(){return m_vPosition;}
void    CParticleSystem::SetPosition(CVector vPosition){m_vPosition=vPosition;}
CVector CParticleSystem::GetAngles(){return m_vAngles;}
void    CParticleSystem::SetAngles(CVector vAngles){m_vAngles=vAngles;m_bAnglesDirty=true;}
void    CParticleSystem::GetVectors(CVector &vForward,CVector &vRight,CVector &vUp)
{
    if(m_bAnglesDirty){VectorsFromAngles(m_vAngles,&m_vForward,&m_vRight,&m_vUp);m_bAnglesDirty=false;}
    vForward=m_vForward;
    vRight=m_vRight;
    vUp=m_vUp;
}