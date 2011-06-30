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
#include "InterfaceLeakAPI.h"
#include "SystemUnknownBase.h"

CSystemUnknownBase::CSystemUnknownBase(void)
{
	//RegisterAddRef(this);
   m_dwReferences=1;
}

CSystemUnknownBase::~CSystemUnknownBase(void)
{
}

void CSystemUnknownBase::AddReference()
{
	//RegisterAddRef(this);
    m_dwReferences++;
}

void CSystemUnknownBase::FinalRelease()
{
}

void CSystemUnknownBase::ReleaseReference()
{
	//RegisterRelease(this);
    CSystemUnknownBase *pThis=this;
    m_dwReferences--;
    if(pThis->m_dwReferences==0)
		{
			FinalRelease();
			delete pThis;
		}
}