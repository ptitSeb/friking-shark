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


#include "./stdafx.h"
#include "OpenGLGraphics.h"
#include "OpenGLFont.h"

bool LoadImageHelper(std::string sFile,unsigned int dwColorType,unsigned *pOpenGLSkinWidth,unsigned *pOpenGLSkinHeight,unsigned char **ppBuffer);

SOpenGLSystemFont::SOpenGLSystemFont()
{
	nMetricDescent=0;
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
		if(!bOk){RTTRACE("COpenGLFont::Init -> Failed to open XWindows Display");}
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
	unsigned dwWidth=0,dwHeight=0;
	bool bResult=true;

	bResult=m_Texture.Create(m_piSystem->GetName(),"Texture","");
	if(bResult){bResult=m_Texture.m_piTexture->Load(m_sTextureFontFileName,NULL,&m_sTextureFontAlphaFileName,1.0);}
	if(bResult){m_Texture.m_piTexture->GetSize(&dwWidth,&dwHeight);}
	if(bResult)
	{
		m_dwTextureWidth=dwWidth;
		m_dwTextureHeight=dwHeight;
		// Se parsean los caracteres de la textura basandonos en el valor del alpha.

		unsigned  nCharacterIndex=0;
		unsigned  nCharacterStartX=0;
		unsigned  nMaxCharacters=(unsigned)strlen(m_sTextureFontCharacterSet.c_str());

		bool bLookingForCharacterStart=true; // si es false se esta buscando el final del caracter.

		for(unsigned int x=0;x<m_dwTextureWidth;x++)
		{
			bool bPixelFound=false;
			for(unsigned int y=0;y<m_dwTextureHeight;y++)
			{
				if(m_Texture.m_piTexture->GetPixelAlpha(x,y)>m_dTextureFontAlphaTolerance)
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

	if(!bResult){RTTRACE("COpenGLFont::Unserialize -> Failed to create texture font from file '%s'",m_sTextureFontFileName.c_str());}
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
				*pdWidth=XTextWidth(pFont->pFontStruct, pText, strlen(pText));
				*pdHeight=pFont->pFontStruct->ascent+pFont->pFontStruct->descent;
			}
#endif
		}
	}
}
void COpenGLFont::RenderText(IGenericRender *piRender,double dFontHeight,double x,double y,const char *pText)
{
	if(m_eFontType==eGenericFontType_Texture)
	{
		double dSizeFactor=dFontHeight/(double)m_dwTextureHeight;

		piRender->PushState();
		piRender->ActivateBlending();
		CVector vOrigin(x,y+dFontHeight,0);
		
		
		int x=0;
		while(pText[x]!=0)
		{
			SOpenGLTextureFontCharacterData *pCharacterData=&m_vTextureFontCharacters[(int)pText[x]];

			if(pText[x]!=' ')
			{
				double s1=pCharacterData->dPixelW*dSizeFactor,s2=pCharacterData->dPixelH*dSizeFactor;
                CVector vCharCenter=vOrigin+piRender->GetCameraRight()*s1*0.5-piRender->GetCameraUp()*s2*0.5;      
				piRender->RenderParticle(m_Texture.m_piTexture,vCharCenter,0,s1,s2,CVector(1,1,1),1.0,pCharacterData->dTextCoordX,pCharacterData->dTextCoordY,pCharacterData->dTextCoordW,pCharacterData->dTextCoordH);
			}

			vOrigin+=piRender->GetCameraRight()*((pCharacterData->dPixelW+m_dTextureFontCharacterSeparation)*dSizeFactor);
			x++;
		}
		
		piRender->PopState();
		
	}
	else
	{
		SOpenGLSystemFont *pFont=GetSystemFontForHeight((unsigned int)dFontHeight);
		if(pFont)
		{
			int nFinalY=(int)(y+pFont->nMetricDescent);
			glRasterPos2d(x,nFinalY);
			//RTTRACE("COpenGLFonts::RenderText -> %s: %d,%d",pText,(int)x,nFinalY);
			
			while(*pText)
			{
				glCallList(pFont->nTexturesBaseIndex+*pText);
				pText++;
			}
		}
	}
}

void COpenGLFont::RenderText(IGenericRender *piRender,double dFontHeight,CVector vPosition,const char *pText)
{
	if(m_eFontType==eGenericFontType_System)
	{
		SOpenGLSystemFont *pFont=GetSystemFontForHeight((unsigned int)dFontHeight);
		if(pFont)
		{
			glRasterPos3d(vPosition.c[0],vPosition.c[1],vPosition.c[2]);
			while(*pText)
			{
				glCallList(pFont->nTexturesBaseIndex+*pText);
				pText++;
			}
		}
	}
	else if(m_eFontType==eGenericFontType_Texture)
	{
		double dSizeFactor=dFontHeight/(double)m_dwTextureHeight;
		
		piRender->PushState();
		piRender->ActivateBlending();
		piRender->DeactivateShadowEmission();
		piRender->DeactivateShadowReception();
		
		CVector vOrigin=vPosition;
		
		
		int x=0;
		while(pText[x]!=0)
		{
			SOpenGLTextureFontCharacterData *pCharacterData=&m_vTextureFontCharacters[(int)pText[x]];
			
			if(pText[x]!=' ')
			{
				double s1=pCharacterData->dPixelW*dSizeFactor,s2=pCharacterData->dPixelH*dSizeFactor;
				CVector vCharCenter=vOrigin+piRender->GetCameraRight()*s1*0.5+piRender->GetCameraUp()*s2*0.5;
				piRender->RenderParticle(m_Texture.m_piTexture,vCharCenter,0,s1,s2,CVector(1,1,1),1.0,pCharacterData->dTextCoordX,pCharacterData->dTextCoordY,pCharacterData->dTextCoordW,pCharacterData->dTextCoordH);
			}
			
			vOrigin+=piRender->GetCameraRight()*((pCharacterData->dPixelW+m_dTextureFontCharacterSeparation)*dSizeFactor);
			x++;
		}
		
		piRender->PopState();
		
	}
}

void COpenGLFont::RenderTextEx(IGenericRender *piRender,double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign)
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
	RenderText(piRender,dFontHeight,dPosX,dPosY,pText);
	//RTTRACE("COpenGLFonts::RenderTextEx -> '%s': Box(%d,%d - %d,%d): H%d,V%d, pos %d,%d",pText,(int)x,(int)y,(int)w,(int)h,dwHorzAlign,dwVertAlign,(int)dPosX,(int)dPosY);
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
			else
			{
				unsigned int nError=GetLastError();
				RTTRACE("COpenGLFont::GetSystemFontForHeight -> wglUseFontBitmaps failed with error code %d ",nError);
			}
			SelectObject(hdc,hOlfFont);
		}
#else
		if(m_pXDisplay!=NULL)
		{
			char sFontName[1024];
			int nLen=sprintf(sFontName,"-*-%s-%s-r-normal--%d-*-*-*-*-*-*-*",m_sSystemFontName.c_str(),m_sSystemFontWeight.c_str(),nHeight);
			for(int x=0;x<nLen;x++){if(sFontName[x]>='A' && sFontName[x]<='Z'){sFontName[x]+='a'-'A';}}
			pFont->pDisplay=m_pXDisplay;
			pFont->pFontStruct=XLoadQueryFont(m_pXDisplay,sFontName);
			if(pFont->pFontStruct)
			{
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
			RTTRACE("COpenGLFont::GetSystemFontForHeight -> Failed to create system font '%s' height %d",m_sSystemFontName.c_str(),nHeight);
			delete pFont;
			pFont=NULL;
		}
	}
	return pFont;
}
