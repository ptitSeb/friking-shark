// OpenGLFonts.h: interface for the COpenGLFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENGLFONTS_H__DE1FEA63_D852_4B3D_961F_FCFD967AE520__INCLUDED_)
#define AFX_OPENGLFONTS_H__DE1FEA63_D852_4B3D_961F_FCFD967AE520__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct SOpenGLFontInfo
{
	char sName[LF_FACESIZE];
	int  nHeight;
	int  nTexturesBaseIndex;
	HFONT hFont;
	TEXTMETRIC sFontMetrics;


	SOpenGLFontInfo()
	{
		hFont=NULL;
		sName[0]=0;
		nHeight=0;
		nTexturesBaseIndex=0;
		memset(&sFontMetrics,0,sizeof(sFontMetrics));
	}

	bool operator <(const SOpenGLFontInfo &font) const
	{
		if(nHeight<font.nHeight){return true;}
		if(nHeight>font.nHeight){return false;}
		return strcmp(sName,font.sName)<0;
	}
	bool operator !=(const SOpenGLFontInfo &font) const
	{
		return (nHeight!=font.nHeight || strcmp(sName,font.sName)!=0);
	}
};

class COpenGLFonts
{
	static std::set<SOpenGLFontInfo> g_sTextures;
	static SOpenGLFontInfo			 g_CurrentFont;

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

#endif // !defined(AFX_OPENGLFONTS_H__DE1FEA63_D852_4B3D_961F_FCFD967AE520__INCLUDED_)
