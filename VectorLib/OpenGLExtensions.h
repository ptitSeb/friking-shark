// OpenGLExtensions.h: interface for the COpenGLExtensions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLEXTENSIONS_H__2551247B_561E_4A58_A30C_E0A884C25DAB__INCLUDED_)
#define AFX_OPENGLEXTENSIONS_H__2551247B_561E_4A58_A30C_E0A884C25DAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )

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

#endif // !defined(AFX_OPENGLEXTENSIONS_H__2551247B_561E_4A58_A30C_E0A884C25DAB__INCLUDED_)
