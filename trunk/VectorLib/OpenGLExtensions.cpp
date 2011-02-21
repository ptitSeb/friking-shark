// OpenGLExtensions.cpp: implementation of the COpenGLExtensions class.
//
//////////////////////////////////////////////////////////////////////
#include "./StdAfx.h"
#include "OpenGLExtensions.h"
#include "Utilities.h"
#include <GL/gl.h>

// OpenGL Extensions
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );

PFNWGLSWAPINTERVALFARPROC wglExtension_SwapInterval = NULL;

std::set<std::string> COpenGLExtensions::g_sExtensions;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLExtensions::COpenGLExtensions()
{

}

COpenGLExtensions::~COpenGLExtensions()
{

}

bool COpenGLExtensions::Init()
{
	const char *pExtensions = (const char *)glGetString( GL_EXTENSIONS );
	if(pExtensions==NULL){return false;}

	int nLen=strlen(pExtensions);
	char *pTempBuffer=new char [nLen+1];
	char *pExt=NULL;
	memcpy(pTempBuffer,pExtensions,nLen+1);
	while((pExt=strtok(pExt?NULL:pTempBuffer," ")))
	{
		std::string sExtensionName=pExt;
		g_sExtensions.insert(sExtensionName);
	//_MRT("COpenGLExtensions::Init -> Extension '%s' found",pExt);
	}
	//_MRT("COpenGLExtensions::Init -> %d Extensions found",g_sExtensions.size());

	delete [] pTempBuffer;

//	if(HasExtension(GLEXT_VSYNC_NAME)){wglExtension_SwapInterval = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress(GLEXT_VSYNC_FUNC);}
	return true;
}

bool COpenGLExtensions::HasExtension(const char *pExtension)
{
	std::string sTemp=pExtension;
	return g_sExtensions.find(sTemp)!=g_sExtensions.end();
}

void COpenGLExtensions::Destroy()
{
}

bool COpenGLExtensions::SetVSync(int bEnabled)
{
  if(wglExtension_SwapInterval==NULL){return false;}
  return wglExtension_SwapInterval(bEnabled)?true:false;
}
