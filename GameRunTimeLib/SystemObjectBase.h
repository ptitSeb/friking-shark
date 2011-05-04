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


class CSystemObjectBase:virtual public CSystemSerializableBase,
                        public CSubscriberBase, 
                        public CPublisherBase,
                        virtual public ISystemObject
{
protected:

    std::string  m_sName;
    std::string  m_sClass;
    ISystem     *m_piSystem;
    
		void FinalRelease();

public:


    virtual bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    virtual void Destroy();

    virtual std::string  GetName();
    virtual std::string  GetClass();
    virtual ISystem     *GetSystem();
    virtual std::string  GetSystemName();

    CSystemObjectBase(void);
    virtual ~CSystemObjectBase(void);
};
