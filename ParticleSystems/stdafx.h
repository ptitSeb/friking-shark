#ifndef __STDAFX__
#define __STDAFX__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "GameLib.h"
#include "GameGraphics.h"

DECLARE_CUSTOM_WRAPPER1(CGenericRenderWrapper,IGenericRender,m_piRender)
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight)

#endif