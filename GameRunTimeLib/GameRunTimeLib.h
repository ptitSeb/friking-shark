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

#include "PlatformDependent.h"

#include <set>
#include <list>
#include <deque>
#include <map>
#include <vector>

#include "GameRunTime.h"
#include "SystemUnknownBase.h"
#include "EventBase.h"
#include "Persistency.h"
#include "PersistentTypes.h"
#include "ConfigFile.h"
#include "SystemObjectBase.h"
#include "SystemModuleHelpers.h"
#include "SystemObjectWrapper.h"
#include "SystemLoaderHelper.h"

template<typename ITF> 
void GetSystemObjects(std::string sSystem,std::vector<ITF *> *pvObjects)
{
	ISystemManager  *piSystemManager=GetSystemManager();
	if(piSystemManager)
	{
		ISystem *piSystem=piSystemManager->GetSystem(sSystem);
		if(piSystem)
		{
			std::vector<ISystemObject*> vObjects;
			piSystem->GetObjects(&vObjects);
			for(unsigned long x=0;x<vObjects.size();x++)
			{
				ISystemObject *piObject=vObjects[x];
				ITF *piITF=QI(ITF,piObject);
				if(piITF){pvObjects->push_back(piITF);}
				REL(piObject);
			}
		}
		REL(piSystem);
	}
	REL(piSystemManager);
}

template<typename T>
T* BufferFromVector(std::vector< T > *pContainer)
{
	T* pBuffer=new T[pContainer->size()];
	T* pBufferCursor=pBuffer;
	typename std::vector<T>::iterator i;
	for(i=pContainer->begin();i!=pContainer->end();i++,pBufferCursor++)
	{
		*pBufferCursor=*i;
	}
	return pBuffer;
}

