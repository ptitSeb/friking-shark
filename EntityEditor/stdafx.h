#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "GameGUI.h"
#include "GameGUILib.h"
#include "GameGraphics.h"
#include "GameManagers.h"
#include "SoundSystems.h"
#include "GameEntityLib.h"
#include "AnimationSystems.h"
#include "ParticleSystems.h"
#include "EntityEditor.h"

DECLARE_CUSTOM_WRAPPER1(CGenericViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CFrameManagerWrapper,IFrameManager,m_piFrameManager)
DECLARE_CUSTOM_WRAPPER1(CGameControllerWrapper,IGameController,m_piGameController)
DECLARE_CUSTOM_WRAPPER2(CPlayAreaManagerWrapper,IPlayAreaManager,m_piPlayAreaManager,IPlayAreaDesign,m_piPlayAreaDesign)
DECLARE_CUSTOM_WRAPPER1(CEntityManagerWrapper,IEntityManager,m_piEntityManager)
DECLARE_CUSTOM_WRAPPER2(CEntityTypeWrapper,IEntityType,m_piEntityType,IEntityTypeDesign,m_piEntityTypeDesign)
DECLARE_CUSTOM_WRAPPER1(CGameRenderWrapper,IGameRender,m_piRender)
DECLARE_CUSTOM_WRAPPER2(CWorldManagerWrapper,ITerrainDesign,m_piTerrain,IWorldManager,m_piWorldManager)
DECLARE_CUSTOM_WRAPPER1(CPhysicManagerWrapper,IPhysicManager,m_piPhysicManager)
DECLARE_CUSTOM_WRAPPER1(CSoundManagerWrapper,ISoundManager,m_piSoundManager)
DECLARE_CUSTOM_WRAPPER2(CAnimationDesignTypeWrapper,IAnimationType,m_piAnimationType,IAnimationTypeDesign,m_piAnimationTypeDesign)
DECLARE_CUSTOM_WRAPPER1(CAnimationEventTypeWrapper,IEventAnimationObjectTypeDesign,m_piDesign)
DECLARE_CUSTOM_WRAPPER1(CAnimationModelTypeWrapper,IModelAnimationObjectTypeDesign,m_piDesign)
DECLARE_CUSTOM_WRAPPER1(CAnimationSoundTypeWrapper,ISoundAnimationObjectTypeDesign,m_piDesign)
DECLARE_CUSTOM_WRAPPER1(CAnimationParticleSystemTypeWrapper,IParticleSystemAnimationObjectTypeDesign,m_piDesign)
DECLARE_CUSTOM_WRAPPER1(CAnimationPositionWrapper,IAnimationObjectTypePositionDesign,m_piDesign)
DECLARE_CUSTOM_WRAPPER1(CAnimationOrientationWrapper,IAnimationObjectTypeOrientationDesign,m_piDesign)


DECLARE_CUSTOM_WRAPPER1(CRenderWrapper,IGenericRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CViewportWrapper,IGenericViewport,m_piViewport)
DECLARE_CUSTOM_WRAPPER1(CButtonWrapper,IGameGUIButton,m_piButton)
DECLARE_CUSTOM_WRAPPER1(CLabelWrapper,IGameGUILabel,m_piLabel)
DECLARE_CUSTOM_WRAPPER1(CObjectLabelWrapper,IEntityEditorObjectLabel,m_piLabel)
DECLARE_CUSTOM_WRAPPER1(CModelWrapper,IGenericModel,m_piModel)
DECLARE_CUSTOM_WRAPPER1(CSoundWrapper,ISoundType,m_piSound)
DECLARE_CUSTOM_WRAPPER1(CParticleSystemWrapper,IParticleSystemType,m_piParticleSystem)

#endif