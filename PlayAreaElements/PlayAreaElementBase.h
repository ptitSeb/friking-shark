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

class CPlayAreaElementBase:virtual public CSystemObjectBase,virtual public IPlayAreaElement
{
protected:

	bool	m_bActive;
    unsigned int   m_nActivationTime;

public:


    virtual void Activate(unsigned int dwCurrentTime);
    virtual void Deactivate();
    virtual bool IsActive();
	virtual void Reset();

		virtual bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pPlayAreaInfo,unsigned int dwCurrentTime,double dInterval);

    CPlayAreaElementBase();
    virtual ~CPlayAreaElementBase();
};

bool Util_IsInPlayArea(CVector vPosition,SPlayAreaInfo *pInfo);
bool Util_IsInPlayArea(CVector vPosition,double dRadius,SPlayAreaInfo *pInfo);

