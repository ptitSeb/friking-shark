// OpenGLFonts.h: interface for the COpenGLFont class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __OPENGL_FONTS__
#define __OPENGL_FONTS__

#ifndef WIN32
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glx.h>
#endif

#define FONT_NAME_MAX 128
struct SOpenGLFontInfo
{
	char sName[FONT_NAME_MAX];
#ifdef WIN32
	HFONT hFont;
	TEXTMETRIC sFontMetrics;
#else
	XFontStruct *pFontStruct;
#endif
	int  nMetricDescent;
	int  nHeight;
	int  nTexturesBaseIndex;
	
	bool operator <(const SOpenGLFontInfo &font) const;
	bool operator !=(const SOpenGLFontInfo &font) const;

	SOpenGLFontInfo();
};

class COpenGLFonts
{
	static std::set<SOpenGLFontInfo> g_sTextures;
	static SOpenGLFontInfo			 g_CurrentFont;
	
#ifndef WIN32
	static Display 					*g_pDisplay;
#endif

	static void DrawText_Internal(char *text);
public:

	static bool		LoadFont(char *sName,int nHeight);
	static void		DrawText(POINT p,const char *format, ...);
	static void		DrawText(double x,double y,const char *format, ...);
	static void		DrawText(char *sName,int nHeight,POINT p,const char *format, ...);
	static void		CalcTextSize(const char *sText,double *pdWidth,double *pdHeight);
	static double	GetCurrentFontHeight();
	static void		SetCurrentFont(char *sName,int nHeight);
	COpenGLFonts();
	virtual ~COpenGLFonts();

};

#endif 
