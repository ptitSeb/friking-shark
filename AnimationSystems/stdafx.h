#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include "GameLib.h"
#include "GameGraphics.h"
#include "ParticleSystems.h"
#include "SoundSystems.h"

DECLARE_CUSTOM_WRAPPER1(CGenericModelWrapper,IGenericModel,m_piModel)
DECLARE_CUSTOM_WRAPPER1(CGenericTextureWrapper,IGenericTexture,m_piTexture)
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight)
DECLARE_CUSTOM_WRAPPER1(CGenericShaderWrapper,IGenericShader,m_piShader)

#endif