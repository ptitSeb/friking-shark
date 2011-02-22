// OpenGLFonts.cpp: implementation of the COpenGLFonts class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include <stdarg.h>
#include "GameLib.h"
#include "OpenGLFonts.h"

std::set<SOpenGLFontInfo> COpenGLFonts::g_sTextures;
		 SOpenGLFontInfo  COpenGLFonts::g_CurrentFont;
#ifndef WIN32
		 Display         *COpenGLFonts::g_pDisplay=NULL;
#endif


SOpenGLFontInfo::SOpenGLFontInfo()
{
	sName[0]=0;
	nMetricDescent=0;
	nHeight=0;
	nTexturesBaseIndex=0;

#ifdef WIN32
	hFont=NULL;
	memset(&sFontMetrics,0,sizeof(sFontMetrics));
#else
	pFontStruct=NULL;
#endif 
}

bool SOpenGLFontInfo::operator <(const SOpenGLFontInfo &font) const
{
	if(nHeight<font.nHeight){return true;}
	if(nHeight>font.nHeight){return false;}
	return strcmp(sName,font.sName)<0;
}
bool SOpenGLFontInfo::operator !=(const SOpenGLFontInfo &font) const
{
	return (nHeight!=font.nHeight || strcmp(sName,font.sName)!=0);
}

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

#ifdef WIN32
	HDC hdc=wglGetCurrentDC();
	LOGFONT logFont={0};
	logFont.lfHeight=MulDiv(nHeight , GetDeviceCaps(hdc, LOGPIXELSY), 72);
	strcpy(logFont.lfFaceName,sName);
	HFONT hFont=CreateFontIndirect(&logFont);
	if(hFont)
	{
		TEXTMETRIC metrics;
		HGDIOBJ hOlfFont=SelectObject(hdc,hFont);
		test.nTexturesBaseIndex=glGenLists(255);
		test.hFont=hFont;
		GetTextMetrics(hdc,&metrics);
		test.nMetricDescent=metrics.tmDescent;
		if(wglUseFontBitmaps(hdc,0,255,test.nTexturesBaseIndex))
		{
			bResult=true;
		}
		else
		{
			if(test.nTexturesBaseIndex){glDeleteLists(test.nTexturesBaseIndex,255);test.nTexturesBaseIndex=0;}
		}
		SelectObject(hdc,hOlfFont);
	}
#else
	if(g_pDisplay==NULL){g_pDisplay=XOpenDisplay(getenv("DISPLAY"));}
	if(g_pDisplay!=NULL)
	{
		char sFontName[1024];
#pragma message ("ver como solucionar el problema del parametro extra de la fuentes en linux")
		
		sprintf(sFontName,"-*-%s-semilight-r-normal--%d-*-*-*-*-*-*-*",sName,nHeight);
		test.nTexturesBaseIndex=glGenLists(255);
		test.pFontStruct=XLoadQueryFont(g_pDisplay,sFontName);
		if(test.pFontStruct)
		{
			//RTTRACE("Font %s:%d ascent %d, descent %d",sName,nHeight,test.pFontStruct->ascent,test.pFontStruct->descent);
			test.nMetricDescent=test.pFontStruct->descent;
			glXUseXFont(test.pFontStruct->fid,0,255,test.nTexturesBaseIndex);
			bResult=true;
		}
		else
		{
			if(test.nTexturesBaseIndex){glDeleteLists(test.nTexturesBaseIndex,255);test.nTexturesBaseIndex=0;}
		}
	}
#endif
	if(bResult)
	{
		g_sTextures.insert(test);
		if(g_CurrentFont.nHeight==0){g_CurrentFont=test;}
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
	int res=vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);
	//RTTRACE("COpenGLFonts::DrawText -> Font %s:%d ascent %d, descent %d",g_CurrentFont.sName,g_CurrentFont.nHeight,g_CurrentFont.pFontStruct->ascent,g_CurrentFont.pFontStruct->descent);

	glRasterPos2d(x,y+g_CurrentFont.nMetricDescent);
	DrawText_Internal(pTempBuffer);
}

void COpenGLFonts::CalcTextSize(const char *sText,double *pdWidth,double *pdHeight)
{
#ifdef WIN32
	HDC hDC=wglGetCurrentDC();
	HGDIOBJ hOlfFont=SelectObject(hDC,g_CurrentFont.hFont);
	RECT rect={0,0,1,1};
	::DrawText(hDC,sText,-1,&rect,DT_CALCRECT|DT_NOCLIP|DT_SINGLELINE);
	*pdWidth=rect.right;
	*pdHeight=rect.bottom;
	SelectObject(hDC,hOlfFont);
#else
	if(g_CurrentFont.pFontStruct)
	{
		int nDirection=0,nAscent=0,nDescent=0;
		XCharStruct overall={0};
		XTextExtents(g_CurrentFont.pFontStruct, sText, strlen(sText), &nDirection, &nAscent, &nDescent, &overall);
		*pdWidth=overall.width;
		*pdHeight=overall.ascent+overall.descent;
		//RTTRACE("COpenGLFonts::CalcTextSize -> Font %s:%d, '%s':%.f x %.f",g_CurrentFont.sName,g_CurrentFont.nHeight,sText,*pdWidth,*pdHeight);
	}
	else
	{
		*pdWidth=*pdHeight=0;
	}
#endif
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
	int res=vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);

	//RTTRACE("COpenGLFonts::DrawText -> Font %s:%d ascent %d, descent %d",g_CurrentFont.sName,g_CurrentFont.nHeight,g_CurrentFont.pFontStruct->ascent,g_CurrentFont.pFontStruct->descent);
	
	glRasterPos2i(p.x,p.y+g_CurrentFont.nMetricDescent);
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
	int res=vsnprintf(pTempBuffer,1023,format,vargs);
	if(res==-1){pTempBuffer[1022]=0;res=1022;}
	va_end (vargs);
	
	//RTTRACE("COpenGLFonts::DrawText -> Font %s:%d ascent %d, descent %d",sName,nHeight,g_CurrentFont.pFontStruct->ascent,g_CurrentFont.pFontStruct->descent);

	glRasterPos2i(p.x,p.y+g_CurrentFont.nMetricDescent);
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