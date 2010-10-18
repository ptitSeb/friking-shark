// OpenGLFonts.cpp: implementation of the COpenGLFonts class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameLib.h"
#include "OpenGLFonts.h"

std::set<SOpenGLFontInfo> COpenGLFonts::g_sTextures;
		 SOpenGLFontInfo  COpenGLFonts::g_CurrentFont;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpenGLFonts::COpenGLFonts(){}
COpenGLFonts::~COpenGLFonts(){}

bool COpenGLFonts::LoadFont(char *sName,int nHeight)
{
	bool bResult=false;
	SOpenGLFontInfo test;
	strcpy(test.sName,sName);
	test.nHeight=nHeight;
	set<SOpenGLFontInfo>::iterator i=g_sTextures.find(test);
	if(i!=g_sTextures.end()){return true;}

	HDC hdc=wglGetCurrentDC();

	LOGFONT logFont={0};
	logFont.lfHeight=nHeight;
	strcpy(logFont.lfFaceName,sName);
	HFONT hFont=CreateFontIndirect(&logFont);
	if(hFont)
	{
		HGDIOBJ hOlfFont=SelectObject(hdc,hFont);
		test.nTexturesBaseIndex=glGenLists(255);
		test.hFont=hFont;
		GetTextMetrics(hdc,&test.sFontMetrics);
		if(wglUseFontBitmaps(hdc,0,255,test.nTexturesBaseIndex))
		{
			g_sTextures.insert(test);
			bResult=true;
			if(g_CurrentFont.nHeight==0)
			{
				g_CurrentFont=test;
			}
		}
		SelectObject(hdc,hOlfFont);
	}
	return bResult;
}

void COpenGLFonts::DrawText_Internal(char *text)
{
	int x=0;
	while(text[x]!=0)
	{
		glCallList(g_CurrentFont.nTexturesBaseIndex+text[x]);
		x++;
	}
}

void COpenGLFonts::DrawText(double x,double y,const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[2400];
	int res=_vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);

	glRasterPos2d(x,y+g_CurrentFont.sFontMetrics.tmDescent);
	DrawText_Internal(pTempBuffer);
}

void COpenGLFonts::CalcTextSize(const char *sText,double *pdWidth,double *pdHeight)
{
	HDC hDC=wglGetCurrentDC();
	HGDIOBJ hOlfFont=SelectObject(hDC,g_CurrentFont.hFont);
	RECT rect={0,0,1,1};
	::DrawText(hDC,sText,-1,&rect,DT_CALCRECT|DT_NOCLIP|DT_SINGLELINE);
	*pdWidth=rect.right;
	*pdHeight=rect.bottom;
	SelectObject(hDC,hOlfFont);
}

double COpenGLFonts::GetCurrentFontHeight()
{
	return g_CurrentFont.nHeight;
}

void COpenGLFonts::DrawText(POINT p,const char *format, ...)
{
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[2400];
	int res=_vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);

	glRasterPos2i(p.x,p.y+g_CurrentFont.sFontMetrics.tmDescent);
	DrawText_Internal(pTempBuffer);
}

void COpenGLFonts::DrawText(char *sName,int nHeight,POINT p,const char *format, ...)
{
	SOpenGLFontInfo test;
	strcpy(test.sName,sName);
	test.nHeight=nHeight;
	if(g_CurrentFont!=test)
	{
		set<SOpenGLFontInfo>::iterator i=g_sTextures.find(test);
		if(i==g_sTextures.end()){return;}
		g_CurrentFont=*i;
	}
	va_list vargs;
	va_start (vargs,format);
	char pTempBuffer[2400];
	int res=_vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);

	glRasterPos2i(p.x,p.y+g_CurrentFont.sFontMetrics.tmDescent);
	DrawText_Internal(pTempBuffer);
}

void  COpenGLFonts::SetCurrentFont(char *sName,int nHeight)
{
	LoadFont(sName,nHeight);
	SOpenGLFontInfo test;
	strcpy(test.sName,sName);
	test.nHeight=nHeight;
	if(g_CurrentFont!=test)
	{
		set<SOpenGLFontInfo>::iterator i=g_sTextures.find(test);
		if(i==g_sTextures.end()){return;}
		g_CurrentFont=*i;
	}
}
