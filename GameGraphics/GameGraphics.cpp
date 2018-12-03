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


// GameGraphics.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"
#include "OpenGLCamera.h"
#include "OpenGLLight.h"
#include "OpenGLRender.h"
#include "OpenGLTexture.h"
#include "OpenGLModel.h"
#include "OpenGLViewport.h"
#include "OpenGLFont.h"
#include "OpenGLShader.h"

BEGIN_SYSTEM_MODULE(GameGraphics)
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLRender,"Render");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLTexture,"Texture");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLModel,"Model");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLFont,"Font");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLViewport,"Viewport");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLCamera,"Camera");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLLight,"Light");
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(COpenGLShader,"Shader");
END_SYSTEM_MODULE(GameGraphics)

