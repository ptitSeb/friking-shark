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


#ifndef _STDAFX_
#define _STDAFX_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <stack>
#include "GameRunTimeLib.h"
#include "VectorLib.h"
#include "GameGraphics.h"
#include "OpenGLGraphics.h"

#define GL_GLEXT_PROTOTYPES
#ifdef ANDROID
	#ifdef ANDROID_GLES1
		#include <GLES/gl.h>
		typedef GLuint GLhandleARB;
		typedef char GLcharARB;
		#define GL_TEXTURE0_ARB GL_TEXTURE0
		
		#define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER
		#define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
		#define GL_DEPTH_ATTACHMENT_EXT GL_DEPTH_ATTACHMENT
		#define GL_COLOR_ATTACHMENT0_EXT GL_COLOR_ATTACHMENT0
		#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE
		#define glClientActiveTextureARB glActiveTexture
		
		#define glBindFramebufferEXT glBindFramebuffer
		#define glBindRenderbufferEXT glBindRenderbuffer
		#define glDeleteFramebuffersEXT glDeleteFramebuffers
		#define glDeleteRenderbuffersEXT glDeleteRenderbuffers
		#define glGenRenderbuffersEXT glGenRenderbuffers
		#define glGenFramebuffersEXT glGenFramebuffers
		#define glFramebufferTexture2DEXT glFramebufferTexture2D
		#define glRenderbufferStorageEXT glRenderbufferStorage
		#define glFramebufferRenderbufferEXT glFramebufferRenderbuffer
		#define glCheckFramebufferStatusEXT glCheckFramebufferStatus
		static void glColorMaterial( GLenum face, GLenum mode ){};
	#else
		#include <GLES2/gl2.h>

		typedef GLuint GLhandleARB;
		typedef char GLcharARB;
		
		#define GL_FRAGMENT_SHADER_ARB GL_FRAGMENT_SHADER
		#define GL_VERTEX_SHADER_ARB GL_VERTEX_SHADER
		#define GL_TEXTURE0_ARB GL_TEXTURE0
			
		#define GL_FRAMEBUFFER_EXT GL_FRAMEBUFFER
		#define GL_RENDERBUFFER_EXT GL_RENDERBUFFER
		#define GL_DEPTH_ATTACHMENT_EXT GL_DEPTH_ATTACHMENT
		#define GL_COLOR_ATTACHMENT0_EXT GL_COLOR_ATTACHMENT0
		#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE
		
		#define glClientActiveTextureARB glActiveTexture
		#define glCreateProgramObjectARB glCreateProgram
		#define glCreateShaderObjectARB glCreateShader
		#define glDetachObjectARB glDetachShader
		#define glCompileShaderARB glCompileShader
		#define glLinkProgramARB glLinkProgram
		#define glGetObjectParameterivARB glGetObjectParameteriv
		#define glGetUniformLocationARB glGetUniformLocation
		#define glUniform1iARB glUniform1i
		#define glUniform1fARB glUniform1f
		#define glAttachObjectARB glAttachShader
		#define glUniform1fvARB glUniform1fv
		#define glUniform2fvARB glUniform2fv
		#define glUniform1ivARB glUniform1iv
		#define glUniform2ivARB glUniform2iv
		#define glUniform3fvARB glUniform3fv
		#define glUniform4fvARB glUniform4fv
		#define glUniformMatrix4fvARB glUniformMatrix4fv
		#define glUseProgramObjectARB glUseProgram
		#define glShaderSourceARB glShaderSource
		
		#define glBindFramebufferEXT glBindFramebuffer
		#define glBindRenderbufferEXT glBindRenderbuffer
		#define glDeleteFramebuffersEXT glDeleteFramebuffers
		#define glDeleteRenderbuffersEXT glDeleteRenderbuffers
		#define glGenRenderbuffersEXT glGenRenderbuffers
		#define glGenFramebuffersEXT glGenFramebuffers
		#define glFramebufferTexture2DEXT glFramebufferTexture2D
		#define glRenderbufferStorageEXT glRenderbufferStorage
		#define glFramebufferRenderbufferEXT glFramebufferRenderbuffer
		#define glCheckFramebufferStatusEXT glCheckFramebufferStatus
	#endif
#else
#include "GLee.h"
#include <GL/glu.h>
#endif

DECLARE_CUSTOM_WRAPPER1(CGenericTextureWrapper,IGenericTexture,m_piTexture)
DECLARE_CUSTOM_WRAPPER1(CGenericLightWrapper,IGenericLight,m_piLight)
DECLARE_CUSTOM_WRAPPER1(CGenericShaderWrapper,IGenericShader,m_piShader)

#endif
