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
#include "SystemModule.h"

#ifdef ANDROID

	#define DECLARE_EMBEDDED_MODULE(name)\
	bool EmbeddedModuleRegister##name(ISystem *piSystem);\
	void EmbeddedModuleUnregister##name(ISystem *piSystem);

	#define BEGIN_EMBEDDED_MODULE_TABLE() \
	typedef bool (*tEmbeddedModuleRegister)(ISystem *piSystem);\
	typedef void (*tEmbeddedModuleUnregister)(ISystem *piSystem);\
	struct SEmbeddedModuleTable\
	{\
	const char *pName;\
	tEmbeddedModuleRegister pfRegister;\
	tEmbeddedModuleUnregister pfUnregister;\
	};\
	SEmbeddedModuleTable g_EmbeddeModuleTable[]={
		
	#define EMBEDDED_MODULE(name) {#name,EmbeddedModuleRegister##name,EmbeddedModuleUnregister##name},
	#define END_EMBEDDED_MODULE_TABLE() {NULL,NULL,NULL}};
		
	DECLARE_EMBEDDED_MODULE(GameGUI)
	DECLARE_EMBEDDED_MODULE(GameManagers)
	DECLARE_EMBEDDED_MODULE(GUISystems)
	DECLARE_EMBEDDED_MODULE(GameGraphics)
	DECLARE_EMBEDDED_MODULE(AnimationSystems)
	DECLARE_EMBEDDED_MODULE(BonusTypes)
	DECLARE_EMBEDDED_MODULE(AirUnitTypes)
	DECLARE_EMBEDDED_MODULE(GroundUnitTypes)
	DECLARE_EMBEDDED_MODULE(ParticleSystems)
	DECLARE_EMBEDDED_MODULE(PlayAreaElements)
	DECLARE_EMBEDDED_MODULE(WeaponTypes)
	DECLARE_EMBEDDED_MODULE(SoundSystems)
	
	BEGIN_EMBEDDED_MODULE_TABLE()
		EMBEDDED_MODULE(GameGUI)
		EMBEDDED_MODULE(GameManagers)
		EMBEDDED_MODULE(GUISystems)
		EMBEDDED_MODULE(GameGraphics)
		EMBEDDED_MODULE(AnimationSystems)
		EMBEDDED_MODULE(BonusTypes)
		EMBEDDED_MODULE(AirUnitTypes)
		EMBEDDED_MODULE(GroundUnitTypes)
		EMBEDDED_MODULE(ParticleSystems)
		EMBEDDED_MODULE(PlayAreaElements)
		EMBEDDED_MODULE(WeaponTypes)
		EMBEDDED_MODULE(SoundSystems)
	END_EMBEDDED_MODULE_TABLE()
#endif


#ifndef WIN32
	#include <dlfcn.h>
	#include "valgrind.h"
#endif

CSystemModule::CSystemModule(void)
{
#ifdef WIN32
	m_hModule=NULL;
#else
	m_pLibrary=NULL;
#endif
	
	m_pSystemModuleRegister=NULL;
	m_pSystemModuleUnregister=NULL;
	
    m_piSystem=NULL;
    m_bRegistered=false;
}

CSystemModule::~CSystemModule(void)
{
}

bool CSystemModule::Init(std::string sPath,ISystem *piSystem)
{
    bool bOk=false;
	std::string sFileName;
    m_sPath=sPath;
	
	ISystemManager *piSystemManager=GetSystemManager();
	
	#ifdef ANDROID
	
	m_pSystemModuleRegister=NULL;
	m_pSystemModuleUnregister=NULL;
	SEmbeddedModuleTable *pCursor=g_EmbeddeModuleTable;
	while(pCursor->pName!=NULL)
	{
		if(strcmp(pCursor->pName,sPath.c_str())==0)
		{
			m_pSystemModuleRegister=pCursor->pfRegister;
			m_pSystemModuleUnregister=pCursor->pfUnregister;
			bOk=true;
			break;
		}
		pCursor++;
	}
	
	#else
		#ifdef WIN32	
			char fileName[MAX_PATH]={0},fileExt[MAX_PATH]={0};
			_splitpath(sPath.c_str(),NULL,NULL,fileName,fileExt);
			sFileName=fileName;
			sFileName+=fileExt;
			sFileName+=".dll";
			
			m_hModule=LoadLibrary(sPath.c_str());
			bOk=(m_hModule!=NULL);
		#else
			const char *pFileStart=strrchr(sPath.c_str(),'/');
			sFileName+=pFileStart?pFileStart:sPath.c_str();
			sFileName+=".so";

			m_pLibrary=dlopen(sPath.c_str(),RTLD_LOCAL|RTLD_LAZY);
			bOk=(m_pLibrary!=NULL);
		#endif
			if(!bOk)
			{
			
			std::set<std::string> sPaths;
			std::set<std::string>::iterator iPaths;
			ISystemManagerPathControl *piPathControl=QI(ISystemManagerPathControl,piSystemManager);
			if(piPathControl)
			{
				piPathControl->GetLibraryPaths(&sPaths);

				for(iPaths=sPaths.begin();iPaths!=sPaths.end();iPaths++)
				{
				std::string sAlternatePath=*iPaths;
				sAlternatePath+=sFileName;
		#ifdef WIN32
				m_hModule=LoadLibrary(sAlternatePath.c_str());
				if(m_hModule!=NULL){bOk=true;break;}
		#else
				m_pLibrary=dlopen(sAlternatePath.c_str(),RTLD_LOCAL|RTLD_LAZY);
				if(m_pLibrary!=NULL){bOk=true;break;}
		#endif
				}
			}
			REL(piPathControl);
			}
		#ifdef WIN32    
			if(!bOk)
			{
			RTTRACE("Failed to load library : %s : %d",sPath.c_str(),GetLastError());
			}

			if(m_hModule)
			{
				m_pSystemModuleRegister=(tSystemModuleRegister)GetProcAddress(m_hModule,"SystemModuleRegister");
				m_pSystemModuleUnregister=(tSystemModuleUnregister)GetProcAddress(m_hModule,"SystemModuleUnregister");
				if(m_pSystemModuleRegister!=NULL && m_pSystemModuleRegister!=NULL){bOk=true;}
				if(!bOk)
				{
				RTTRACE("Failed to get library procedures: %s : %d",sPath.c_str(),GetLastError());
				}
			}
		#else
			if(!bOk)
			{
			RTTRACE("Failed to load library : %s : %s",sPath.c_str(),dlerror());
			}

			if(m_pLibrary)
			{
				m_pSystemModuleRegister=reinterpret_cast<tSystemModuleRegister>(reinterpret_cast<size_t>(dlsym(m_pLibrary,"SystemModuleRegister")));
				m_pSystemModuleUnregister=reinterpret_cast<tSystemModuleUnregister>(reinterpret_cast<size_t>(dlsym(m_pLibrary,"SystemModuleUnregister")));
				if(m_pSystemModuleRegister!=NULL && m_pSystemModuleRegister!=NULL){bOk=true;}
				if(!bOk)
				{
				RTTRACE("Failed to get library procedures: %s : %s",sPath.c_str(),dlerror());
				}
			}
		#endif
	#endif

	if(bOk){bOk=m_bRegistered=piSystem->RegisterModule(this);}
	if(bOk){bOk=m_pSystemModuleRegister(piSystem);}
	if(bOk){m_piSystem=ADD(piSystem);}
	if(!bOk)
	{
		if(m_bRegistered){piSystem->UnregisterModule(this);m_bRegistered=false;}
#ifdef WIN32
		if(m_hModule){FreeLibrary(m_hModule);m_hModule=NULL;}
#elif defined LINUX
		if(m_pLibrary){dlclose(m_pLibrary);m_pLibrary=NULL;}
#endif
		m_pSystemModuleRegister=NULL;
		m_pSystemModuleUnregister=NULL;        
		m_sPath="";
	}

    REL(piSystemManager);
    return bOk;
}

void CSystemModule::Destroy()
{
	if(m_pSystemModuleUnregister){m_pSystemModuleUnregister(m_piSystem);}
	if(m_bRegistered){m_piSystem->UnregisterModule(this);m_bRegistered=false;}
		
	#ifdef WIN32
		if(m_hModule){FreeLibrary(m_hModule);m_hModule=NULL;}
	#elif defined LINUX
		if(!RUNNING_ON_VALGRIND)
		{
			// Do not release shared libraries if running on valgrind, otherwise symbols are not available when dumping memory leaks.
			if(m_pLibrary){dlclose(m_pLibrary);m_pLibrary=NULL;}
		}
	#endif
	
	REL(m_piSystem);
	m_pSystemModuleRegister=NULL;
	m_pSystemModuleUnregister=NULL;

}

std::string CSystemModule::GetName()
{
    return m_sPath;
}
