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


#pragma once
#include "GameGraphics.h"

struct SOpenGLTextureFontCharacterData
{
	bool   bPresent;
	char   cCharacter;
	double dTextCoordX;
	double dTextCoordY;
	double dTextCoordW;
	double dTextCoordH;
	double dPixelW;
	double dPixelH;

	SOpenGLTextureFontCharacterData()
	{
		bPresent=false;
		cCharacter=0;
		dTextCoordX=dTextCoordY=dTextCoordW=dTextCoordH=dPixelH=dPixelW=0;
	}
};

class COpenGLFont : virtual public CSystemObjectBase,virtual public IGenericFont
{
	
	CGenericTextureWrapper m_Texture;
	
	unsigned 	 		m_dwTextureWidth;
	unsigned 	 		m_dwTextureHeight;

	SOpenGLTextureFontCharacterData m_vTextureFontCharacters[256];

	std::string		m_sTextureFontFileName;
	std::string		m_sTextureFontCharacterSet;
	double			m_dTextureFontCharacterSeparation;
	double			m_dTextureFontSpaceSize;
	double			m_dTextureFontAlphaTolerance;
	unsigned int	m_nTextureFontRowHeight;
	unsigned int	m_nTextureFontRowCount;
	bool			m_bTextureFontGenerateMipmaps;
	
	unsigned int	m_nTextureFontEffectiveHeight;
	
	bool LoadTextureFont();
	bool Unserialize(ISystemPersistencyNode *piNode);
	
public:

	BEGIN_PROP_MAP(COpenGLFont)
		PROP_VALUE_FLAGS(m_nTextureFontRowHeight,"RowHeight",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nTextureFontRowCount,"RowCount",1,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_sTextureFontFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_sTextureFontCharacterSet,"JuegoDeCaracteres",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dTextureFontCharacterSeparation,"SeparacionCaracteres",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dTextureFontSpaceSize,"TamanoEspacio",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dTextureFontAlphaTolerance,"ToleranciaAlpha",0,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_bTextureFontGenerateMipmaps,"GenerateMipmaps",true,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();

	void CalcTextSize(double dFontHeight,const char *pText,double *pdWidth,double *pdHeight);
	void RenderText(IGenericRender *piRender,double dFontHeight,double x,double y,const char *pText,const CVector &vColor,double dAlpha);
	void RenderText(IGenericRender *piRender,double dFontHeight,CVector vPosition,const char *pText,const CVector &vColor,double dAlpha);
	void RenderTextEx(IGenericRender *piRender,double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign,const CVector &vColor,double dAlpha);

	COpenGLFont(void);
	~COpenGLFont(void);
};
