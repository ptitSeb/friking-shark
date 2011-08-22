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

#include "GameRunTime.h"
#include "GameRunTimeLib.h"
#include <map>

class CSystemManager:   virtual public CSystemUnknownBase,
                        public CPublisherBase,
                        public ISystemManager,
                        public ISystemManagerPathControl
{
    std::map<std::string,ISystem*> m_mSystems;
    std::set<std::string>          m_sLibraryPaths;
	std::map<ISystemObject *,std::string>       m_mDebugObjects;

public:

  // ISystemManager

    ISystem *CreateSystem(std::string sName);
    void    *DestroySystem(ISystem *piSystem);

    ISystem *GetSystem(std::string sName);

    bool    RegisterSystem(ISystem *piSystem);
    void    UnregisterSystem(ISystem *piSystem);
	
	void			DebugRegisterObject(ISystemObject *piObject);
	void			DebugUnregisterObject(ISystemObject *piObject);
	unsigned long	DebugGetRegisteredObjectCount();
	void			DebugGetRegisteredObjects(std::map<ISystemObject *,std::string> *pObjects);

    // ISystemManagerPathControl

    void AddLibraryPath(std::string sPath);
    void RemoveLibraryPath(std::string sPath);
    void ClearLibraryPaths();
    void GetLibraryPaths(std::set<std::string> *pvPaths);

    CSystemManager(void);
    ~CSystemManager(void);

    static CSystemManager *g_pSystemManager;
};

