#include "./stdafx.h"
#include "OpenGLGraphics.h"
#include "OpenGLFont.h"

bool LoadImageHelper(string sFile,DWORD dwColorType,unsigned *pOpenGLSkinWidth,unsigned *pOpenGLSkinHeight,BYTE **ppBuffer);

SOpenGLSystemFont::SOpenGLSystemFont()
{
	nMetricDescent=0;
	nHeight=0;
	nTexturesBaseIndex=0;
#ifdef WIN32
	hFont=NULL;
	memset(&sFontMetrics,0,sizeof(sFontMetrics));
#else
	pDisplay=NULL;
	pFontStruct=NULL;
#endif 
}

SOpenGLSystemFont::~SOpenGLSystemFont()
{
#ifdef WIN32
	if(nTexturesBaseIndex){glDeleteLists(nTexturesBaseIndex,255);nTexturesBaseIndex=0;}
	if(hFont){DeleteObject(hFont);hFont=NULL;}
#else
	if(pFontStruct){XFreeFont(pDisplay,pFontStruct);pFontStruct=NULL;}
#endif
}

COpenGLFont::COpenGLFont(void)
{
	m_dwTextureWidth=0;
	m_dwTextureHeight=0;
	m_pTextureBuffer=NULL;
	m_nTextureIndex=0;

	m_eFontType=eGenericFontType_UNKNOWN;
	m_dTextureFontCharacterSeparation=0;
	m_dTextureFontSpaceSize=0;
	m_dTextureFontAlphaTolerance=0;

#ifndef WIN32
	m_pXDisplay=NULL;
#endif
}

COpenGLFont::~COpenGLFont(void)
{
	if(m_pTextureBuffer)
	{
		delete [] m_pTextureBuffer;
		m_pTextureBuffer=NULL;
	}
}
eGenericFontType COpenGLFont::GetType(){return m_eFontType;}

bool COpenGLFont::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
#ifndef WIN32
	if(bOk)
	{
		m_pXDisplay=XOpenDisplay(getenv("DISPLAY"));
		bOk=(m_pXDisplay!=NULL);
	}
#endif
	return bOk;
}

void COpenGLFont::Destroy()
{
	for(std::map<unsigned int,SOpenGLSystemFont*>::iterator i=m_mSystemFontHeights.begin();i!=m_mSystemFontHeights.end();i++)
	{
		SOpenGLSystemFont *pFont=i->second;
		delete  pFont;
	}
	m_mSystemFontHeights.clear();
#ifndef WIN32
	if(m_pXDisplay){XCloseDisplay(m_pXDisplay);m_pXDisplay=NULL;}
#endif
	CSystemObjectBase::Destroy();
}

bool COpenGLFont::LoadTextureFont()
{
	BYTE *pBuffer=NULL;
	unsigned dwWidth=0,dwHeight=0;
	bool bResult=true;

	if(LoadImageHelper(m_sTextureFontFileName,GL_RGB,&dwWidth,&dwHeight,&pBuffer))
	{
		m_dwTextureWidth=dwWidth;
		m_dwTextureHeight=dwHeight/2;
		m_pTextureBuffer=new BYTE[m_dwTextureWidth*m_dwTextureHeight*4];

		// Se monta la textura final con la mascara alpha de la mitad inferior aplicado a la mitad superior.

		BYTE *pTempFinal=m_pTextureBuffer;
		BYTE *pTempBuffer=pBuffer+(dwWidth*(dwHeight/2))*3; // mitad superior de la textura
		BYTE *pTempAlpha=pBuffer; // mitad inferior de la textura

		unsigned x,y;
		for(y=0;y<m_dwTextureHeight;y++)
		{
			for(x=0;x<m_dwTextureWidth;x++,pTempBuffer+=3,pTempAlpha+=3,pTempFinal+=4)
			{
				pTempFinal[0]=pTempBuffer[0];
				pTempFinal[1]=pTempBuffer[1];
				pTempFinal[2]=pTempBuffer[2];
				pTempFinal[3] = (BYTE)((((DWORD)pTempAlpha[0])+((DWORD)pTempAlpha[1])+((DWORD)pTempAlpha[2]))/3);
			}
		}

		// Se parsean los caracteres de la textura basandonos en el valor del alpha.

		unsigned  nCharacterIndex=0;
		unsigned  nCharacterStartX=0;
		unsigned  nMaxCharacters=(unsigned)strlen(m_sTextureFontCharacterSet.c_str());

		bool bLookingForCharacterStart=true; // si es false se esta buscando el final del caracter.

		for(x=0;x<m_dwTextureWidth;x++)
		{
			bool bPixelFound=false;
			for(y=0;y<m_dwTextureHeight;y++)
			{
				int nPos=(x+y*m_dwTextureWidth)*4;
				if(m_pTextureBuffer[nPos+3]>m_dTextureFontAlphaTolerance)
				{
					bPixelFound=true;
					break;
				}
			}
			if(bLookingForCharacterStart && bPixelFound)
			{
				bLookingForCharacterStart=false;
				nCharacterStartX=x;
			}
			else if(bLookingForCharacterStart==false && !bPixelFound)
			{
				bLookingForCharacterStart=true;
				unsigned char cChar=m_sTextureFontCharacterSet.c_str()[nCharacterIndex];
				SOpenGLTextureFontCharacterData *pData=&m_vTextureFontCharacters[cChar];
				pData->bPresent=true;
				pData->cCharacter=cChar;
				pData->dPixelW=x-nCharacterStartX;
				pData->dPixelH=m_dwTextureHeight;
				pData->dTextCoordY=0;
				pData->dTextCoordH=1;
				pData->dTextCoordX=(double)nCharacterStartX/(double)m_dwTextureWidth;
				pData->dTextCoordW=pData->dPixelW/(double)m_dwTextureWidth;
				nCharacterIndex++;
			}
			if(nCharacterIndex>=nMaxCharacters){break;}
		}
	}
	else
	{
		bResult=false;
	}

	m_vTextureFontCharacters[' '].dPixelW=m_dTextureFontSpaceSize;
	m_vTextureFontCharacters[' '].dPixelH=m_dwTextureHeight;
	m_vTextureFontCharacters[' '].bPresent=true;

	if(pBuffer){delete [] pBuffer;pBuffer=NULL;}

	if(bResult)
	{
		glGenTextures(1,&m_nTextureIndex);

		if(m_nTextureIndex)
		{
			glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			gluBuild2DMipmaps(GL_TEXTURE_2D,4,m_dwTextureWidth,m_dwTextureHeight,GL_RGBA,GL_UNSIGNED_BYTE,m_pTextureBuffer);
		}
	}
	return bResult;
}
bool COpenGLFont::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	if(m_eFontType==eGenericFontType_Texture)
	{
		bResult=LoadTextureFont();
	}
	return bResult;
}

void COpenGLFont::CalcTextSize(double dFontHeight,const char *pText,double *pdWidth,double *pdHeight)
{
	double dSizeFactor=dFontHeight/(double)m_dwTextureHeight;

	(*pdWidth)=0;
	(*pdHeight)=0;
	if(m_eFontType==eGenericFontType_Texture)
	{
		int x=0;
		while(pText[x]!=0)
		{
			double dCharWidth,dCharHeight;
			dCharWidth=m_vTextureFontCharacters[(int)pText[x]].dPixelW*dSizeFactor;
			dCharHeight=m_vTextureFontCharacters[(int)pText[x]].dPixelH*dSizeFactor;
			(*pdWidth)+=dCharWidth;
			if(x!=0){(*pdWidth)+=m_dTextureFontCharacterSeparation*dSizeFactor;}
			if(dCharHeight>(*pdHeight)){(*pdHeight)=dCharHeight;}
			x++;
		}
	}
	else
	{
		SOpenGLSystemFont *pFont=GetSystemFontForHeight((unsigned int)dFontHeight);
		if(pFont)
		{
#ifdef WIN32
			HDC hDC=wglGetCurrentDC();
			HGDIOBJ hOlfFont=SelectObject(hDC,pFont->hFont);
			RECT rect={0,0,1,1};
			::DrawText(hDC,pText,-1,&rect,DT_CALCRECT|DT_NOCLIP|DT_SINGLELINE);
			*pdWidth=rect.right;
			*pdHeight=rect.bottom;
			SelectObject(hDC,hOlfFont);
#else
			if(pFont->pFontStruct)
			{
				int nDirection=0,nAscent=0,nDescent=0;
				XCharStruct overall={0};
				XTextExtents(pFont->pFontStruct, pText, strlen(pText), &nDirection, &nAscent, &nDescent, &overall);
				*pdWidth=overall.width;
				*pdHeight=overall.ascent+overall.descent;
				//RTTRACE("COpenGLFonts::CalcTextSize -> Font %s:%d, '%s':%.f x %.f",g_CurrentFont.sName,g_CurrentFont.nHeight,sText,*pdWidth,*pdHeight);
			}
#endif
		}
	}
}


void COpenGLFont::RenderText(double dFontHeight,double x,double y,const char *pText)
{
	if(m_eFontType==eGenericFontType_Texture)
	{
		double dSizeFactor=dFontHeight/(double)m_dwTextureHeight;

		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D,m_nTextureIndex);

		CVector vOrigin(x,y+dFontHeight,0);
		CVector vOrientation(CVector(1,0,0));

		int x=0;
		while(pText[x]!=0)
		{
			CVector vAxis2=vOrientation^AxisPosZ;
			SOpenGLTextureFontCharacterData *pCharacterData=&m_vTextureFontCharacters[(int)pText[x]];

			if(pText[x]!=' ')
			{
				double s1=pCharacterData->dPixelW*dSizeFactor,s2=pCharacterData->dPixelH*dSizeFactor;

				glBegin(GL_QUADS);
				glTexCoord2d(pCharacterData->dTextCoordX+pCharacterData->dTextCoordW,pCharacterData->dTextCoordY+pCharacterData->dTextCoordH);
				glVertex3dv((vOrigin+vOrientation*s1).c);
				glTexCoord2d(pCharacterData->dTextCoordX,pCharacterData->dTextCoordY+pCharacterData->dTextCoordH);
				glVertex3dv(vOrigin.c);
				glTexCoord2d(pCharacterData->dTextCoordX,pCharacterData->dTextCoordY);
				glVertex3dv((vOrigin+vAxis2*s2).c);
				glTexCoord2d(pCharacterData->dTextCoordX+pCharacterData->dTextCoordW,pCharacterData->dTextCoordY);
				glVertex3dv((vOrigin+vOrientation*s1+vAxis2*s2).c);
				glEnd();
			}

			vOrigin.c[0]+=(pCharacterData->dPixelW+m_dTextureFontCharacterSeparation)*dSizeFactor;
			x++;
		}

		glDisable(GL_TEXTURE_2D);
		glPopAttrib();
	}
	else
	{
		SOpenGLSystemFont *pFont=GetSystemFontForHeight((unsigned int)dFontHeight);
		if(pFont)
		{
			glRasterPos2d(x,y+pFont->nMetricDescent);
			while(*pText)
			{
				glCallList(pFont->nTexturesBaseIndex+*pText);
				pText++;
			}
		}
	}
}

void COpenGLFont::RenderTextEx(double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign)
{
	double dTextW=0,dTextH=0;
	double dPosX=0,dPosY=0;

	CalcTextSize(dFontHeight,pText,&dTextW,&dTextH);

	switch(dwHorzAlign)
	{
	case eTextAlignment_None:
	case eTextAlignment_Left:
		dPosX=x;
		break;
	case eTextAlignment_Right:
		dPosX=(x+w)-dTextW;
		break;
	case eTextAlignment_Center:
		dPosX=x+(w*0.5)-(dTextW*0.5);
		break;
	}
	switch(dwVertAlign)
	{
	case eTextAlignment_None:
	case eTextAlignment_Top:
		dPosY=(y+h)-dTextH;
		break;
	case eTextAlignment_Bottom:
		dPosY=y;
		break;
	case eTextAlignment_Center:
		dPosY=y+(h*0.5)-(dTextH*0.5);
		break;
	}
	RenderText(dFontHeight,dPosX,dPosY,pText);
	//RTTRACE("COpenGLFonts::RenderTextEx -> W: %d,%d - %d,%d: '%s', H%d,V%d",(int)x,(int)y,(int)w,(int)h,pText,dwHorzAlign,dwVertAlign);
	//RTTRACE("COpenGLFonts::RenderTextEx -> T: %d x %d",(int)dTextW,(int)dTextH);
	//RTTRACE("COpenGLFonts::RenderTextEx -> P: %d x %d",(int)dPosX,(int)dPosY);
}

SOpenGLSystemFont *COpenGLFont::GetSystemFontForHeight(unsigned int nHeight)
{
	std::map<unsigned int,SOpenGLSystemFont*>::iterator i=m_mSystemFontHeights.find(nHeight);
	SOpenGLSystemFont *pFont=(i!=m_mSystemFontHeights.end())?i->second:NULL;
	if(pFont==NULL)
	{
		pFont=new SOpenGLSystemFont;
		pFont->nHeight=nHeight;
		pFont->nTexturesBaseIndex=glGenLists(255);

		bool bOk=false;
#ifdef WIN32
		HDC hdc=wglGetCurrentDC();
		LOGFONT logFont={0};
		logFont.lfHeight=MulDiv(nHeight , GetDeviceCaps(hdc, LOGPIXELSY), 72);
		pFont->hFont=CreateFontIndirect(&logFont);
		if(pFont->hFont)
		{
			TEXTMETRIC metrics;
			HGDIOBJ hOlfFont=SelectObject(hdc,pFont->hFont);
			GetTextMetrics(hdc,&metrics);
			pFont->nMetricDescent=metrics.tmDescent;
			if(wglUseFontBitmaps(hdc,0,255,pFont->nTexturesBaseIndex))
			{
				bOk=true;
			}
			SelectObject(hdc,hOlfFont);
		}
#else
		if(m_pXDisplay!=NULL)
		{
			#pragma message ("ver como solucionar el problema del parametro extra de la fuentes en linux")
			
			char sFontName[1024];
			sprintf(sFontName,"-*-%s-semilight-r-normal--%d-*-*-*-*-*-*-*",m_sSystemFontName.c_str(),nHeight);
			pFont->pDisplay=m_pXDisplay;
			pFont->pFontStruct=XLoadQueryFont(m_pXDisplay,sFontName);
			if(pFont->pFontStruct)
			{
				//RTTRACE("Font %s:%d ascent %d, descent %d",sName,nHeight,test.pFontStruct->ascent,test.pFontStruct->descent);
				pFont->nMetricDescent=pFont->pFontStruct->descent;
				glXUseXFont(pFont->pFontStruct->fid,0,255,pFont->nTexturesBaseIndex);
				bOk=true;
			}
		}
#endif
		if(bOk)
		{
			m_mSystemFontHeights[nHeight]=pFont;
		}
		else
		{
			delete pFont;
			pFont=NULL;
		}
	}
	return pFont;
}
