// OpenGLExtensions.h: interface for the COpenGLExtensions class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _OPENGL_EXTENSIONS_
#define _OPENGL_EXTENSIONS_

#ifdef WIN32
	#pragma warning ( disable : 4786 )
#endif

#include <set>
#include <string>

#define GLEXT_VSYNC_NAME "WGL_EXT_swap_control"
#define GLEXT_VSYNC_FUNC "wglSwapIntervalEXT"

class COpenGLExtensions  
{
	static std::set<std::string> g_sExtensions;
public:

	static bool HasExtension(const char *pExtensionName);
	static bool SetVSync(int bEnabled=0);

	static bool Init();
	static void Destroy();

	COpenGLExtensions();
	virtual ~COpenGLExtensions();

};

#endif 
