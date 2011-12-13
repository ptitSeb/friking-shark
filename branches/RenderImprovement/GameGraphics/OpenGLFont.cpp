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

COpenGLFont::COpenGLFont(void)
{
	m_bTextureFontGenerateMipmaps=true;
	m_nTextureFontEffectiveHeight=0;
	
	m_dwTextureWidth=0;
	m_dwTextureHeight=0;

	m_nTextureFontRowHeight=0;
	m_nTextureFontRowCount=0;
	
	m_dTextureFontCharacterSeparation=0;
	m_dTextureFontSpaceSize=0;
	m_dTextureFontAlphaTolerance=0;
}

COpenGLFont::~COpenGLFont(void)
{
}

bool COpenGLFont::LoadTextureFont()
{
	unsigned dwWidth=0,dwHeight=0;
	bool bResult=true;

	bResult=m_Texture.Create(m_piSystem->GetName(),"Texture","");
	if(bResult){bResult=m_Texture.m_piTexture->Load(m_sTextureFontFileName,NULL,&m_sTextureFontAlphaFileName,1.0,m_bTextureFontGenerateMipmaps,true);}
	if(bResult){m_Texture.m_piTexture->GetSize(&dwWidth,&dwHeight);}
	if(bResult)
	{
		m_dwTextureWidth=dwWidth;
		m_dwTextureHeight=dwHeight;
		m_nTextureFontEffectiveHeight=m_nTextureFontRowHeight?m_nTextureFontRowHeight:dwHeight;
		
		// Se parsean los caracteres de la textura basandonos en el valor del alpha.

		unsigned  nCharacterIndex=0;
		unsigned  nCharacterStartX=0;
		unsigned  nMaxCharacters=(unsigned)strlen(m_sTextureFontCharacterSet.c_str());

		bool bLookingForCharacterStart=true; // si es false se esta buscando el final del caracter.

		unsigned int nRowStart=0;
		
		for(unsigned int r=0;r<m_nTextureFontRowCount && nRowStart<m_dwTextureHeight;r++,nRowStart+=m_nTextureFontEffectiveHeight)
		{
			bLookingForCharacterStart=true;
			
			for(unsigned int x=0;x<m_dwTextureWidth;x++)
			{
				bool bPixelFound=false;
				for(unsigned int y=0;y<m_nTextureFontEffectiveHeight && (y+nRowStart)<m_dwTextureHeight;y++)
				{
					if(m_Texture.m_piTexture->GetPixelAlpha(x,y+nRowStart)>m_dTextureFontAlphaTolerance)
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
					pData->dPixelH=m_nTextureFontEffectiveHeight;
					pData->dTextCoordY=((double)(nRowStart))/(double)m_dwTextureHeight;
					pData->dTextCoordH=((double)(m_nTextureFontEffectiveHeight))/(double)m_dwTextureHeight;
					pData->dTextCoordX=(double)nCharacterStartX/(double)m_dwTextureWidth;
					pData->dTextCoordW=pData->dPixelW/(double)m_dwTextureWidth;
					nCharacterIndex++;
				}
				if(nCharacterIndex>=nMaxCharacters){break;}
			}
		}
	
		m_vTextureFontCharacters[' '].dPixelW=m_dTextureFontSpaceSize;
		m_vTextureFontCharacters[' '].dPixelH=m_nTextureFontEffectiveHeight;
		m_vTextureFontCharacters[' '].bPresent=true;
	}
	else
	{
		bResult=false;
	}


	if(m_Texture.m_piTexture){m_Texture.m_piTexture->ReleaseResidentData();}
	if(!bResult){RTTRACE("COpenGLFont::Unserialize -> Failed to create texture font from file '%s'",m_sTextureFontFileName.c_str());}
	return bResult;
}

bool COpenGLFont::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	if(bResult){bResult=LoadTextureFont();}
	return bResult;
}

void COpenGLFont::CalcTextSize(double dFontHeight,const char *pText,double *pdWidth,double *pdHeight)
{
	double dSizeFactor=dFontHeight/(double)m_nTextureFontEffectiveHeight;

	(*pdWidth)=0;
	(*pdHeight)=0;

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
void COpenGLFont::RenderText(IGenericRender *piRender,double dFontHeight,double x,double y,const char *pText,const CVector &vColor,double dAlpha)
{
	double dSizeFactor=dFontHeight/(double)m_nTextureFontEffectiveHeight;

	piRender->PushState();
	piRender->ActivateBlending();
	CVector vOrigin(floor(x),floor(y+dFontHeight),0);
	
	bool bTextureSelected=false;
	
	int i=0;
	while(pText[i]!=0)
	{
		SOpenGLTextureFontCharacterData *pCharacterData=&m_vTextureFontCharacters[(int)pText[i]];

		if(pText[i]!=' ')
		{
			double s1=pCharacterData->dPixelW*dSizeFactor,s2=pCharacterData->dPixelH*dSizeFactor;
			CVector vCharCenter=vOrigin+piRender->GetCameraRight()*s1*0.5-piRender->GetCameraUp()*s2*0.5;
			if(!bTextureSelected)
			{
				bTextureSelected=true;
				piRender->SelectTexture(m_Texture.m_piTexture,0);
			}
			piRender->RenderTexture(vCharCenter,s1,s2,pCharacterData->dTextCoordX,pCharacterData->dTextCoordY,pCharacterData->dTextCoordW,pCharacterData->dTextCoordH,vColor,dAlpha);
		}

		vOrigin+=piRender->GetCameraRight()*((pCharacterData->dPixelW+m_dTextureFontCharacterSeparation)*dSizeFactor);
		i++;
	}
	
	if(bTextureSelected){piRender->UnselectTexture(0);}
	
	piRender->PopState();
}

void COpenGLFont::RenderText(IGenericRender *piRender,double dFontHeight,CVector vPosition,const char *pText,const CVector &vColor,double dAlpha)
{
	double dSizeFactor=dFontHeight/(double)m_nTextureFontEffectiveHeight;
	
	piRender->PushState();
	piRender->ActivateBlending();
	piRender->DeactivateShadowEmission();
	piRender->DeactivateShadowReception();
	
	CVector vOrigin=vPosition;
	
	bool bTextureSelected=false;
	int i=0;
	while(pText[i]!=0)
	{
		SOpenGLTextureFontCharacterData *pCharacterData=&m_vTextureFontCharacters[(int)pText[i]];
		
		if(pText[i]!=' ')
		{
			double s1=pCharacterData->dPixelW*dSizeFactor,s2=pCharacterData->dPixelH*dSizeFactor;
			CVector vCharCenter=vOrigin+piRender->GetCameraRight()*s1*0.5+piRender->GetCameraUp()*s2*0.5;
			if(!bTextureSelected)
			{
				bTextureSelected=true;
				piRender->SelectTexture(m_Texture.m_piTexture,0);
			}
			piRender->RenderTexture(vCharCenter,s1,s2,pCharacterData->dTextCoordX,pCharacterData->dTextCoordY,pCharacterData->dTextCoordW,pCharacterData->dTextCoordH,vColor,dAlpha);
		}
		
		vOrigin+=piRender->GetCameraRight()*((pCharacterData->dPixelW+m_dTextureFontCharacterSeparation)*dSizeFactor);
		i++;
	}
	
	if(bTextureSelected){piRender->UnselectTexture(0);}
	piRender->PopState();
}

void COpenGLFont::RenderTextEx(IGenericRender *piRender,double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign,const CVector &vColor,double dAlpha)
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
	RenderText(piRender,dFontHeight,dPosX,dPosY,pText,vColor,dAlpha);
	//RTTRACE("COpenGLFonts::RenderTextEx -> '%s': Box(%d,%d - %d,%d): H%d,V%d, pos %d,%d",pText,(int)x,(int)y,(int)w,(int)h,dwHorzAlign,dwVertAlign,(int)dPosX,(int)dPosY);
	//RTTRACE("COpenGLFonts::RenderTextEx -> T: %d x %d",(int)dTextW,(int)dTextH);
	//RTTRACE("COpenGLFonts::RenderTextEx -> P: %d x %d",(int)dPosX,(int)dPosY);
}
