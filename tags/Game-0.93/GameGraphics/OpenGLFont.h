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

#define FONT_NAME_MAX 128
struct SOpenGLSystemFont
{
#ifdef WIN32
	HFONT hFont;
	TEXTMETRIC sFontMetrics;
#else
	XFontStruct *pFontStruct;
	Display     *pDisplay;
#endif
	int  nTexturesBaseIndex;
	int  nMetricDescent;

	SOpenGLSystemFont();
	~SOpenGLSystemFont();
};

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

/*
Clase de dibujo de fuentes. 

Puede mostrar dos tipos de fuentes:
	- Fuentes de sistema, es decir, fuentes instaladas en el sistema que se obtienen mediante las funciones de wgl/glx.
	- Fuentes de textura, es decir, fuentes cargadas a partir de un bitmap en el que aparece un juego concreto de caracteres.
	  El bitmap debe estar dispuesto como dos filas de dibujos de caracteres.
		En la fila superior se encuentra el dibujo de los caracteres.
		En la fila inferior se encuentran las mascaras alpha.
		El juego de caracteres se indica con una cadena de texto en "JuegoDeCaracteres".
*/

class COpenGLFont : virtual public CSystemObjectBase,virtual public IGenericFont
{
	CGenericTextureWrapper m_Texture;
	
	unsigned 	 		m_dwTextureWidth;
	unsigned 	 		m_dwTextureHeight;

	SOpenGLTextureFontCharacterData m_vTextureFontCharacters[256];
	std::map<unsigned int,SOpenGLSystemFont*> m_mSystemFontHeights;

	eGenericFontType m_eFontType;

	std::string		m_sTextureFontFileName;
	std::string		m_sTextureFontAlphaFileName;
	std::string		m_sTextureFontCharacterSet;
	double			m_dTextureFontCharacterSeparation;
	double			m_dTextureFontSpaceSize;
	double			m_dTextureFontAlphaTolerance;

#ifndef WIN32
	Display 		*m_pXDisplay;
#endif
	std::string		m_sSystemFontName;
	std::string 	m_sSystemFontWeight;

	bool LoadTextureFont();
	bool Unserialize(ISystemPersistencyNode *piNode);

	SOpenGLSystemFont *GetSystemFontForHeight(unsigned int nHeight);

public:

	BEGIN_PROP_MAP(COpenGLFont)
		PROP_FLAGS(m_eFontType,"Tipo",MRPF_NORMAL)
		PROP_FLAGS(m_sTextureFontFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_sTextureFontAlphaFileName,"ArchivoAlpha",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_sTextureFontCharacterSet,"JuegoDeCaracteres",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dTextureFontCharacterSeparation,"SeparacionCaracteres",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dTextureFontSpaceSize,"TamanoEspacio",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dTextureFontAlphaTolerance,"ToleranciaAlpha",0,MRPF_NORMAL|MRPF_OPTIONAL);

		PROP_FLAGS(m_sSystemFontName,"Familia",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_sSystemFontWeight,"Grosor","*",MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	eGenericFontType GetType();

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	void CalcTextSize(double dFontHeight,const char *pText,double *pdWidth,double *pdHeight);
	void RenderText(IGenericRender *piRender,double dFontHeight,double x,double y,const char *pText);
	void RenderText(IGenericRender *piRender,double dFontHeight,CVector vPosition,const char *pText);
	void RenderTextEx(IGenericRender *piRender,double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign);

	COpenGLFont(void);
	~COpenGLFont(void);
};
