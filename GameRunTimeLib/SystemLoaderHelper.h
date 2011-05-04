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

struct SSystemLoaderHelperModule
{
    std::string sPath;
};

BEGIN_STRUCT_PROPS(SSystemLoaderHelperModule)
    PROP(sPath,"Path")
END_STRUCT_PROPS()

struct CSystemLoaderHelperModuleList
{
    std::deque<SSystemLoaderHelperModule> m_dModules;

    BEGIN_PROP_MAP(CSystemLoaderHelperModuleList)
        PROP(m_dModules,"Modules")
    END_PROP_MAP()
};


class CDelayedUnserializeObjectWrapper: public CSystemObjectWrapper
{
protected:
	ISystemPersistencyNode	*m_piNode;
	
	bool UnserializeObject(ISystemPersistencyNode *piNode);
	
public:
	
	bool UnserializeNow();
	
	CDelayedUnserializeObjectWrapper();
	~CDelayedUnserializeObjectWrapper();
	
};

bool MRPersistencySave(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
bool MRPersistencyLoad(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
bool MRPersistencyRemove(ISystemPersistencyNode *piNode,CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
void MRPersistencyInitialize(CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);
void MRPersistencyFree(CMRPersistentReferenceT<CDelayedUnserializeObjectWrapper> *pItem);

class CSystemLoaderHelper
{
    CSystemLoaderHelperModuleList    m_Modules;
	std::deque<CDelayedUnserializeObjectWrapper> m_dObjects;
	
	
public:

    ISystem *LoadSystem(ISystemPersistencyNode *piNode,std::string sSystemName);
	ISystem *LoadSystem(std::string sFile,std::string sSystemName);
	
    BEGIN_PROP_MAP(CSystemLoaderHelper)
        PROP(m_dObjects,"Objects")
    END_PROP_MAP()
    
    CSystemLoaderHelper(void);
    virtual ~CSystemLoaderHelper(void);
};

class CSystemSaverHelper
{
	std::deque<SSystemLoaderHelperModule> m_dModules;
	std::deque<CSystemObjectWrapper> m_dObjects;
	
public:
	
	bool SaveSystemNamedObjects(std::string sFile,std::string sSystemName);
	bool SaveSystemNamedObjects(ISystemPersistencyNode *piNode,std::string sSystemName);
	
	BEGIN_PROP_MAP(CSystemSaverHelper)
		PROP_FLAGS(m_dModules,"Modules",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dObjects,"Objects",MRPF_WRITE)
	END_PROP_MAP()
	
	CSystemSaverHelper(void);
	~CSystemSaverHelper(void);
};
