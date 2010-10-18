#include "StdAfx.h"
#include "OpenGLGraphics.h"
#include "OpenGLFont.h"

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

bool COpenGLFont::LoadTextureFont()
{
	BYTE *pBuffer=NULL;
	unsigned dwWidth=0,dwHeight=0;
	bool bResult=true;

	if(LoadTextureImageHelper(m_sTextureFontFileName,GL_RGB,&dwWidth,&dwHeight,&pBuffer))
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
				SOpenGLFontCharacterData *pData=&m_vTextureFontCharacters[cChar];
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
			dCharWidth=m_vTextureFontCharacters[pText[x]].dPixelW*dSizeFactor;
			dCharHeight=m_vTextureFontCharacters[pText[x]].dPixelH*dSizeFactor;
			(*pdWidth)+=dCharWidth;
			if(x!=0){(*pdWidth)+=m_dTextureFontCharacterSeparation*dSizeFactor;}
			if(dCharHeight>(*pdHeight)){(*pdHeight)=dCharHeight;}
			x++;
		}
	}
	else
	{
		COpenGLFonts::LoadFont(const_cast<char*>(m_sSystemFontName.c_str()),(int)dFontHeight);
		COpenGLFonts::CalcTextSize(pText,pdWidth,pdHeight);
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
			SOpenGLFontCharacterData *pCharacterData=&m_vTextureFontCharacters[pText[x]];

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
		COpenGLFonts::LoadFont(const_cast<char*>(m_sSystemFontName.c_str()),(int)dFontHeight);
		COpenGLFonts::DrawText(x,y,pText);
	}
}

void COpenGLFont::RenderTextEx(double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign)
{
	double dTextW=0,dTextH=0;
	double dPosX=0,dPosY=0;

	CalcTextSize(dFontHeight,pText,&dTextW,&dTextH);

	switch(dwHorzAlign)
	{
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
}
