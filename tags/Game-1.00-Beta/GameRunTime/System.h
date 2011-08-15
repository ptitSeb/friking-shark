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

#include <map>

class CSystem:public CSystemObjectBase,public ISystem
{
    ISystemManager  *m_piSystemManager;
    ISystem         *m_piSystem;

    std::map<std::string,ISystemObject*>     m_mObjects;
    std::map<std::string,ISystemClass*>      m_mClasses;
    std::map<std::string,ISystemModule*>     m_mModules;

public:

    // ISystemObject

    virtual bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    virtual void Destroy();

    // ISystem

    virtual bool RegisterClassFactory(ISystemClass *piClass);
    virtual void UnregisterClassFactory(ISystemClass *piClass);

    virtual bool LoadModule(std::string sPath,ISystemModule **ppiModule);
    virtual bool GetModule(std::string sPath,ISystemModule **ppiModule);
	virtual void GetModules(std::vector<ISystemModule *> *pvModules);
	virtual bool RegisterModule(ISystemModule *piModule);
    virtual void UnregisterModule(ISystemModule *piModule);

    virtual bool RegisterObject(ISystemObject *piObject);
    virtual void UnregisterObject(ISystemObject *piObject);

    virtual bool GetObject(std::string sName,ISystemObject **piObject);
	virtual void GetObjects(std::vector<ISystemObject *> *pvObjects);

    virtual bool CreateObject(std::string sClass,ISystemObject **piObject);
    virtual bool CreateObject(std::string sClass,std::string sName,ISystemObject **piObject);

	virtual bool GetClass(std::string sNameClass,ISystemClass **piClass);
	virtual void GetClasses(std::vector<ISystemClass *> *pvClasses);
	

	virtual void DestroyAllObjects();

     CSystem(ISystemManager *piSystemManager);
    ~CSystem(void);
};
