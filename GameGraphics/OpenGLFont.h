#pragma once
#include "GameGraphics.h"

struct SOpenGLFontCharacterData
{
	bool   bPresent;
	char   cCharacter;
	double dTextCoordX;
	double dTextCoordY;
	double dTextCoordW;
	double dTextCoordH;
	double dPixelW;
	double dPixelH;

	SOpenGLFontCharacterData()
	{
		bPresent=false;
		cCharacter=0;
		dTextCoordX=dTextCoordY=dTextCoordW=dTextCoordH=dPixelH=dPixelW=0;
	}
};
/*
Clase de dibujo de fuentes. 

Puede mostrar dos tipos de fuentes:
	- Fuentes de sistema, es decir, fuentes instaladas en el sistema que se obtienen mediante las funciones de wgl.
	- Fuentes de textura, es decir, fuentes cargadas a partir de un bitmap en el que aparece un juego concreto de caracteres.
	  El bitmap debe estar dispuesto como dos filas de dibujos de caracteres.
		En la fila superior se encuentra el dibujo de los caracteres.
		En la fila inferior se encuentran las mascaras alpha.
		El juego de caracteres se indica con una cadena de texto en "JuegoDeCaracteres".
*/
class COpenGLFont : virtual public CSystemObjectBase,virtual public IGenericFont
{
	unsigned m_dwTextureWidth;
	unsigned m_dwTextureHeight;
	BYTE		*m_pTextureBuffer;
	unsigned m_nTextureIndex;

	SOpenGLFontCharacterData m_vTextureFontCharacters[256];

	eGenericFontType m_eFontType;

	std::string		m_sTextureFontFileName;
	std::string		m_sTextureFontCharacterSet;
	double				m_dTextureFontCharacterSeparation;
	double				m_dTextureFontSpaceSize;
	double				m_dTextureFontAlphaTolerance;

	std::string		m_sSystemFontName;

	bool LoadTextureFont();
	bool Unserialize(ISystemPersistencyNode *piNode);

public:

	BEGIN_PROP_MAP(COpenGLFont)
		PROP_FLAGS(m_eFontType,"Tipo",MRPF_NORMAL)
		
		PROP_FLAGS(m_sTextureFontFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_sTextureFontCharacterSet,"JuegoDeCaracteres",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dTextureFontCharacterSeparation,"SeparacionCaracteres",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_dTextureFontSpaceSize,"TamañoEspacio",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dTextureFontAlphaTolerance,"ToleranciaAlpha",0,MRPF_NORMAL|MRPF_OPTIONAL);

		PROP_FLAGS(m_sSystemFontName,"Familia",MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	eGenericFontType GetType();

	void CalcTextSize(double dFontHeight,const char *pText,double *pdWidth,double *pdHeight);
	void RenderText(double dFontHeight,double x,double y,const char *pText);
	void RenderTextEx(double dFontHeight,double x,double y,double w,double h,const char *pText,eTextAlignment dwHorzAlign,eTextAlignment dwVertAlign);

	COpenGLFont(void);
	~COpenGLFont(void);
};
