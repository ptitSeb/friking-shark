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

class CSystemModule:virtual public CSystemUnknownBase,
                    virtual public ISystemModule
{
    std::string m_sPath;
#ifdef WIN32
    HMODULE     m_hModule;
#else
    void*       m_pLibrary;
#endif
    ISystem     *m_piSystem;
    bool        m_bRegistered;

    typedef bool (*tSystemModuleRegister)(ISystem *);
    typedef bool (*tSystemModuleUnregister)(ISystem *);

    tSystemModuleRegister    m_pSystemModuleRegister;
    tSystemModuleUnregister  m_pSystemModuleUnregister;
public:

    virtual bool Init(std::string sPath,ISystem *piSystem);
    virtual void Destroy();

    virtual std::string GetName();

    CSystemModule(void);
    ~CSystemModule(void);
};
