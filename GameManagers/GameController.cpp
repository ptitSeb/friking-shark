#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameController.h"

CGameController::CGameController()
{
	m_piResourcesSystem=NULL;
	m_piParticleSystemTypesSystem=NULL;
	m_piAnimationTypesSystem=NULL;
	m_piEntityTypesSystem=NULL;
	m_piFormationTypesSystem=NULL;
	m_piWeaponTypesSystem=NULL;
}

CGameController::~CGameController()
{

}

bool CGameController::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
    return bOk;
}

void CGameController::Destroy()
{
	Stop();
	EndGame();

    map<DWORD,IGameManager *>::iterator i;
    for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
    {
        IGameManager *piManager=i->second;
        REL(piManager);
    }
    m_mManagers.clear();


    CSystemObjectBase::Destroy();
}

bool CGameController::SetupGame()
{
	CSystemLoaderHelper     systemLoader;
	m_piResourcesSystem=systemLoader.LoadSystem("Scripts/GameResources.cfg","GameResources");
	m_piParticleSystemTypesSystem=systemLoader.LoadSystem("Scripts/ParticleSystems.cfg","ParticleSystems");
	m_piAnimationTypesSystem=systemLoader.LoadSystem("Scripts/Animations.cfg","Animations");
	m_piWeaponTypesSystem=systemLoader.LoadSystem("Scripts/WeaponTypes.cfg","WeaponTypes");
	m_piEntityTypesSystem=systemLoader.LoadSystem("Scripts/EntityTypes.cfg","EntityTypes");
	m_piFormationTypesSystem=systemLoader.LoadSystem("Scripts/FormationTypes.cfg","FormationTypes");
	return true;
}

void CGameController::EndGame()
{
	Stop();

	if(m_piEntityTypesSystem){m_piEntityTypesSystem->DestroyAllObjects();}
	if(m_piFormationTypesSystem){m_piFormationTypesSystem->DestroyAllObjects();}
	if(m_piWeaponTypesSystem){m_piWeaponTypesSystem->DestroyAllObjects();}
	if(m_piAnimationTypesSystem){m_piAnimationTypesSystem->DestroyAllObjects();}
	if(m_piParticleSystemTypesSystem){m_piParticleSystemTypesSystem->DestroyAllObjects();}

	if(m_piEntityTypesSystem){m_piEntityTypesSystem->Destroy();}
	if(m_piFormationTypesSystem){m_piFormationTypesSystem->Destroy();}
	if(m_piWeaponTypesSystem){m_piWeaponTypesSystem->Destroy();}
	if(m_piAnimationTypesSystem){m_piAnimationTypesSystem->Destroy();}
	if(m_piParticleSystemTypesSystem){m_piParticleSystemTypesSystem->Destroy();}
	if(m_piResourcesSystem){m_piResourcesSystem->Destroy();}

	REL(m_piEntityTypesSystem);
	REL(m_piFormationTypesSystem);
	REL(m_piWeaponTypesSystem);	
	REL(m_piAnimationTypesSystem);
	REL(m_piParticleSystemTypesSystem);
	REL(m_piResourcesSystem);
}

void CGameController::CreateScenario()
{
	CloseScenario();

	map<DWORD,IGameManager *>::iterator i;
	for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
	{
		IGameManager *piManager=i->second;
		piManager->CreateScenario();
	}
}

bool CGameController::LoadScenario(std::string sScenario)
{
	CloseScenario();

	CConfigFile configFile;
	bool bOk=configFile.Open(sScenario);
	if(bOk)
	{
		map<DWORD,IGameManager *>::iterator i;
		for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
		{
			IGameManager *piManager=i->second;
			piManager->LoadScenario(configFile.GetRoot());
		}
	}
	return bOk;
}
bool CGameController::SaveScenario(std::string sScenario)
{
	CConfigFile configFile;
	map<DWORD,IGameManager *>::iterator i;
	for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
	{
		IGameManager *piManager=i->second;
		piManager->SaveScenario(configFile.GetRoot());
	}
	return configFile.Save(sScenario);
}

void CGameController::CloseScenario()
{
	Stop();

    map<DWORD,IGameManager *>::iterator i;
    for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
    {
        IGameManager *piManager=i->second;
        piManager->CloseScenario();
    }
}

void CGameController::Start()
{
	Stop();

	map<DWORD,IGameManager *>::iterator i;
	for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
	{
		IGameManager *piManager=i->second;
		piManager->Start();
	}
}

void CGameController::Stop()
{
	map<DWORD,IGameManager *>::reverse_iterator i;
	for(i=m_mManagers.rbegin();i!=m_mManagers.rend();i++)
	{
		IGameManager *piManager=i->second;
		piManager->Stop();
	}
}

void CGameController::ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)
{
	if(dTimeFraction==0.0){return;}

	map<DWORD,IGameManager *>::iterator i;
    for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
    {
        IGameManager *piManager=i->second;
        piManager->ProcessFrame(dwCurrentTime,dTimeFraction);
    }
}

bool CGameController::RegisterManager(DWORD dwHeight,IGameManager *piManager)
{
    if(m_mManagers.find(dwHeight)!=m_mManagers.end()){return false;}
    ADD(piManager);
    m_mManagers[dwHeight]=piManager;
    return true;
}

void CGameController::UnregisterManager(IGameManager *piManager)
{
    map<DWORD,IGameManager *>::iterator i;
    for(i=m_mManagers.begin();i!=m_mManagers.end();i++)
    {
        IGameManager *piTempManager=i->second;
        if(piTempManager==piManager)
        {
            m_mManagers.erase(i);
            REL(piTempManager);
            break;
        }
    }
}
