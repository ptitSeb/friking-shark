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


// AnimationSystems.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationType.h"
#include "AnimationObjectBase.h"
#include "ModelAnimationObjectType.h"
#include "EventAnimationObjectType.h"
#include "ParticleSystemAnimationObjectType.h"
#include "EntityAnimationObjectType.h"
#include "SoundAnimationObjectType.h"
#include "TextAnimationObjectType.h"

BEGIN_SYSTEM_MODULE()
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CAnimationType,"AnimationType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CModelAnimationObjectType,"ModelAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEventAnimationObjectType,"EventAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleSystemAnimationObjectType,"ParticleSystemAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityAnimationObjectType,"EntityAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSoundAnimationObjectType,"SoundAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CTextAnimationObjectType,"TextAnimationObjectType")
END_SYSTEM_MODULE()

