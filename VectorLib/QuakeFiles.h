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


// QuakeFiles.h: interface for the FileTypes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUAKEFILES_H__A699036E_2DC3_416F_8541_70AA9F2CECAD__INCLUDED_)
#define AFX_QUAKEFILES_H__A699036E_2DC3_416F_8541_70AA9F2CECAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////
/////////////////		MDL FILES
/////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

	#include "VectorLib.h"

	#define MAX_SKINS 20
	#define MAX_IMAGENES 100
	#define MAX_FRAMES 500

	struct SMdlHeader
	{
		long nID;
		long nVersion;
		float pfScale[3];
		float pfOrigin[3];
		float fRadius;
		float pfEjePos[3];
		long  nSkins;
		long  nSkinWidth;
		long  nSkinHeight;
		
		long  nVertexes;
		long  nTriangles;
		long  nFrames;
		
		long  nSync;
		long  nFlags;
		float fSize;
		
		
		SMdlHeader(){memset(this,0,sizeof(*this));};
		~SMdlHeader(){};
	};
	
	// estructura de skin simple.
	
	struct SMdlSkin
	{
		unsigned char *pPixels; // aqui se amlacenan los pixels.
		
		SMdlSkin(){pPixels=NULL;}
		~SMdlSkin(){if(pPixels!=NULL){delete [] pPixels;}pPixels=NULL;}
	};
	
	struct SMdlSkinVertex
	{
		long		nBack; // vertice de la parte delantera o trasera
		long		x; // posicion en el skin
		long		y;
		
		SMdlSkinVertex(){x=y=nBack=0;};
		~SMdlSkinVertex(){};
	};
	
	struct SMdlTriangle
	{
		long		nFront;
		long		pnVertexIndexes[3];
		
		SMdlTriangle(){nFront=false;pnVertexIndexes[0]=pnVertexIndexes[1]=pnVertexIndexes[2]=0;};
		~SMdlTriangle(){};
	};
	
	struct SMdlCompressedVertex
	{
		unsigned char x;
		unsigned char y;
		unsigned char z;
		unsigned char nNormalIndex;
		
		SMdlCompressedVertex(){x=y=z=nNormalIndex=0;}
		~SMdlCompressedVertex(){};
	};
	
	struct SMdlFrame
	{
		SMdlCompressedVertex 	  m_nMinVertex;
		SMdlCompressedVertex 	  m_nMaxVertex;
		
		char m_sName[16];
		
		SMdlCompressedVertex 	 *pnVertexIndexes;
		
		SMdlFrame(){pnVertexIndexes=NULL;m_sName[0]=0;};
		~SMdlFrame()
		{
			if(pnVertexIndexes!=NULL){delete [] pnVertexIndexes;pnVertexIndexes=NULL;}
		};
	};
	
	struct SMdlPaletteColor
	{
		unsigned char cRed;
		unsigned char cGreen;
		unsigned char cBlue;
		
		SMdlPaletteColor(){cRed=0;cGreen=0;cBlue=0;};
		~SMdlPaletteColor(){};
	};
	
	class CMdlFileType
	{
								
	public:
		SMdlHeader 			m_Header;
		SMdlSkin			 *m_pSkins[MAX_SKINS];
		SMdlSkinVertex		 *m_pSkinVertexes;
		SMdlTriangle		 *m_pTriangles;
		SMdlFrame			 *m_pFrames[MAX_FRAMES];
		long				m_nOpenGLSkinWidth;
		long				m_nOpenGLSkinHeigth;

		int		m_nSkins; 		// skins simples
		int		m_nFrames; 		// frames simples

		CPolyhedron *GetPolyhedron(int frame,int skin);
		
		bool Open(const char *pFileName);
		void Close();
		
		CMdlFileType();
		~CMdlFileType();
	};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////
/////////////////		MAP FILES
/////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class CMapFilePolygon: public CMaterialPolygon
	{
	public:

		char m_sTextName[MAX_PATH];

		 CMapFilePolygon(CPolygon &polygon);
		~CMapFilePolygon();
	};

	class CMapFileBrush
	{
	public:
			std::vector<CMapFilePolygon *> m_vPolygons;

		 CMapFileBrush();
		~CMapFileBrush();
	};

	class CMapFileType  
	{
		CMapFileBrush *ReadBrush(char *pBuffer,unsigned int *pOffset,unsigned int fileLen,double size);

		void SkipCommentsStringsAndSpaces(char *pBuffer,unsigned int *pOffset,unsigned int bufLen);
	public:
		
		std::list<CMapFileBrush *>	m_lBrushes;

		bool Open(const char *pFile);
		void Close();

		CMapFileType();
		virtual ~CMapFileType();

	};

extern SMdlPaletteColor *g_pPaletteLmp;

#endif // !defined(AFX_FILETYPES_H__A699036E_2DC3_416F_8541_70AA9F2CECAD__INCLUDED_)
