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

BEGIN_SYSTEM_MODULE()
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CAnimationType,"AnimationType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CModelAnimationObjectType,"ModelAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEventAnimationObjectType,"EventAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleSystemAnimationObjectType,"ParticleSystemAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CEntityAnimationObjectType,"EntityAnimationObjectType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CSoundAnimationObjectType,"SoundAnimationObjectType")
END_SYSTEM_MODULE()

