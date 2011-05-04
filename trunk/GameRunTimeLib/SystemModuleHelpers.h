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

template<typename CLASS>
class CSystemClassHelperT: public CSystemUnknownBase,public ISystemClass
{
public:
    std::string           m_sName;
    virtual bool CreateInstance(ISystemObject **ppiObject){*ppiObject=new CLASS;return true;}
    virtual std::string GetName(){return m_sName;}
    CSystemClassHelperT(std::string sName)
	{
		m_sName=sName;
	}
};

class CSystemModuleHelper
{
    typedef void (*tBuildClassMap)(CSystemModuleHelper *pModuleHelper);

    ISystemClass *m_pSystemClasses[256];
    int           m_nSystemClasses;

public:
	
	void AddClass(ISystemClass *piClass);
    void RegisterClasses(ISystem *piSystem);
    void UnregisterClasses(ISystem *piSystem);

    CSystemModuleHelper(tBuildClassMap pBuildClassMap);
    ~CSystemModuleHelper();
};

#ifdef WIN32
#define BEGIN_SYSTEM_MODULE() \
    CSystemModuleHelper *g_pSystemModuleHelper=NULL;\
    void CSystemModuleHelper_BuildClassMap(CSystemModuleHelper *pModuleHelper)\
    {
#define SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLASS,NAME) {ISystemClass *piClass=new CSystemClassHelperT<CLASS>(NAME);pModuleHelper->AddClass(piClass);REL(piClass);}
#define SYSTEM_MODULE_CUSTOM_CLASS_FACTORY_ENTRY(INSTANCE) pModuleHelper->AddClass(INSTANCE);REL(INSTANCE);
#define END_SYSTEM_MODULE()\
    }\
    extern "C" \
    {\
        bool  __declspec(dllexport) SystemModuleRegister(ISystem *piSystem){g_pSystemModuleHelper->RegisterClasses(piSystem);return true;}\
        void  __declspec(dllexport) SystemModuleUnregister(ISystem *piSystem){g_pSystemModuleHelper->UnregisterClasses(piSystem);}\
    }\
    BOOL APIENTRY DllMain( HANDLE hModule, unsigned int  ul_reason_for_call, LPVOID lpReserved)\
    {\
    switch (ul_reason_for_call)\
        {\
        case DLL_PROCESS_ATTACH:g_pSystemModuleHelper=new CSystemModuleHelper(CSystemModuleHelper_BuildClassMap);break;\
        case DLL_PROCESS_DETACH:if(g_pSystemModuleHelper){delete g_pSystemModuleHelper;g_pSystemModuleHelper=NULL;}break;\
        }\
        return TRUE;\
    }
#else
#define BEGIN_SYSTEM_MODULE() \
    void CSystemModuleHelper_BuildClassMap(CSystemModuleHelper *pModuleHelper)\
    {
#define SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLASS,NAME) {ISystemClass *piClass=new CSystemClassHelperT<CLASS>(NAME);pModuleHelper->AddClass(piClass);REL(piClass);}
#define SYSTEM_MODULE_CUSTOM_CLASS_FACTORY_ENTRY(INSTANCE) pModuleHelper->AddClass(INSTANCE);REL(INSTANCE);
#define END_SYSTEM_MODULE()\
    }\
    CSystemModuleHelper g_SystemModuleHelper(CSystemModuleHelper_BuildClassMap);\
    extern "C" \
    {\
        bool SystemModuleRegister(ISystem *piSystem){g_SystemModuleHelper.RegisterClasses(piSystem);return true;}\
        void SystemModuleUnregister(ISystem *piSystem){g_SystemModuleHelper.UnregisterClasses(piSystem);}\
    }
#endif
