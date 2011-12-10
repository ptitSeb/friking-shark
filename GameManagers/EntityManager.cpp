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
#include "GameRunTimeLib.h"
#include "EntityManager.h"

CEntityManager::CEntityManager()
{
}

CEntityManager::~CEntityManager()
{
}

bool CEntityManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
    if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
    if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(400,this);}
    return bOk;
}

void CEntityManager::Destroy()
{
    if(m_GameControllerWrapper.m_piGameController)
    {
        m_GameControllerWrapper.m_piGameController->UnregisterManager(this);
    }
    CSystemObjectBase::Destroy();
}

void CEntityManager::CreateScenario(){}
void CEntityManager::LoadScenario(ISystemPersistencyNode *piNode){}
void CEntityManager::SaveScenario(ISystemPersistencyNode *piNode){}
void CEntityManager::CloseScenario(){}
void CEntityManager::Start(){}
void CEntityManager::Stop()
{
    RemoveAllEntities();
}

void CEntityManager::AddEntity(IEntity *piEntity)
{
    m_lEntities.push_back(piEntity);
}

void CEntityManager::RemoveEntity(IEntity *piEntity)
{
    list<IEntity *>::iterator i;
    for(i=m_lEntities.begin();i!=m_lEntities.end();i++)
    {
        if(*i==piEntity)
        {
            delete piEntity;
            m_lEntities.erase(i);
            return;
        }
    }
}

void CEntityManager::RemoveAllEntities()
{
    list<IEntity *>::iterator i;
    for(i=m_lEntities.begin();i!=m_lEntities.end();i++)
    {
        IEntity *piEntity=*i;
		// Se llama a remove principalemente para que se notifique a los suscriptores la eliminacion de la entidad
		piEntity->Remove(); 
        delete piEntity;
    }
    m_lEntities.clear();
}

unsigned long CEntityManager::GetEntityCount()
{
	return (unsigned long)m_lEntities.size();
}

void CEntityManager::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	RTTIMEMETER_SETSTEP("Managers-Entity");
    // Run 
    list<IEntity *>::iterator i;
    for(i=m_lEntities.begin();i!=m_lEntities.end();)
    {
      IEntity *piEntity=*i;
      bool bAnimationsFinished=false;
      bool bDeleted=false;
      bool bRemoved=piEntity->IsRemoved();

      unsigned int   dwNextProcessFrame=piEntity->GetNextProcessFrame();
      if(!bRemoved)
      {
        if(dwNextProcessFrame && dwCurrentTime>=dwNextProcessFrame)
        {
          piEntity->ProcessFrame(dwCurrentTime,dTimeFraction);
        }
      }
      piEntity->ProcessAnimations(dwCurrentTime,dTimeFraction,&bAnimationsFinished);
      if(bRemoved)
      {
        if(bAnimationsFinished)
        {
          i=m_lEntities.erase(i);
          delete piEntity;
          bDeleted=true;
        }
      }
      if(!bDeleted){i++;}
    }
    for(unsigned x=0;x<m_lNewEntities.size();x++)
    {
        IEntity *piEntity=m_lNewEntities[x];
        if(piEntity->IsRemoved())
        {
            delete piEntity;			
        }
        else
        {
            m_lEntities.push_back(piEntity);
        }
    }
    m_lNewEntities.clear();
	RTTIMEMETER_ENDSTEP();
	
}

void CEntityManager::PerformUnaryOperation(EntityUnaryOperation pFunc,void *pParam1,void *pParam2)
{
    list<IEntity *>::iterator i;
    for(i=m_lEntities.begin();i!=m_lEntities.end();i++)
    {
        IEntity *piEntity=*i;
        pFunc(piEntity,pParam1,pParam2);
    }
}

void CEntityManager::PerformBinaryOperation(EntityBinaryOperation pFunc,void *pParam1,void *pParam2)
{
    list<IEntity *>::iterator i;
    list<IEntity *>::iterator i2;
    for(i=m_lEntities.begin();i!=m_lEntities.end();i++)
    {
        IEntity *piEntity1=*i;
        for(i2=m_lEntities.begin();i2!=m_lEntities.end();i2++)
        {
            IEntity *piEntity2=*i2;
            pFunc(piEntity1,piEntity2,pParam1,pParam2);
        }
    }
}

IEntity *CEntityManager::FindEntity(string sName)
{
    list<IEntity *>::iterator i;
    for(i=m_lEntities.begin();i!=m_lEntities.end();i++)
    {
        IEntity *piEntity=*i;
        if(*piEntity->GetEntityName()==sName){return piEntity;}
    }
    return NULL;
}

void CEntityManager::RenderEntities(IGenericRender *piRender,IGenericCamera *piCamera)
{
	list<IEntity *>::iterator i;
	for(i=m_lEntities.begin();i!=m_lEntities.end();i++)
	{
		IEntity *piEntity=*i;
		piEntity->Render(piRender,piCamera);
	}
}