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


// QuakeFiles.cpp: implementation of the FileTypes class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include "QuakeFiles.h"
#include "float.h"
#include <string>

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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
unsigned short g_pPaletteLmpBuffer[]=
{
    0x0000, 0x0f00, 0x0f0f, 0x1f1f, 0x2f1f, 0x2f2f, 0x3f3f, 0x4b3f, 0x4b4b, 
    0x5b5b, 0x6b5b, 0x6b6b, 0x7b7b, 0x8b7b, 0x8b8b, 0x9b9b, 0xab9b, 0xabab, 
    0xbbbb, 0xcbbb, 0xcbcb, 0xdbdb, 0xebdb, 0xebeb, 0x0b0f, 0x1707, 0x0b0f, 
    0x171f, 0x270b, 0x0f1b, 0x232f, 0x3713, 0x172b, 0x2f3f, 0x4b17, 0x1b37, 
    0x3b53, 0x5b1b, 0x1f43, 0x4b63, 0x6b1f, 0x1f53, 0x5773, 0x7b1f, 0x235f, 
    0x6783, 0x8f23, 0x236f, 0x0b0b, 0x130f, 0x1b13, 0x1b1b, 0x2727, 0x3327, 
    0x2f2f, 0x373f, 0x4b37, 0x3f3f, 0x4757, 0x6747, 0x4f4f, 0x5b73, 0x7f5b, 
    0x6363, 0x6b8b, 0x976b, 0x7373, 0x7ba3, 0xaf7b, 0x8383, 0x8bbb, 0xcb8b, 
    0x0000, 0x0700, 0x0007, 0x0b0b, 0x1300, 0x0013, 0x1b1b, 0x2300, 0x0023, 
    0x2b2b, 0x2f07, 0x072f, 0x3737, 0x3f07, 0x073f, 0x4747, 0x4b07, 0x0b4b, 
    0x5353, 0x5b0b, 0x0b5b, 0x6363, 0x6b0b, 0x0f6b, 0x0007, 0x0f00, 0x0000, 
    0x0017, 0x1f00, 0x0000, 0x0027, 0x2f00, 0x0000, 0x0037, 0x3f00, 0x0000, 
    0x0047, 0x4f00, 0x0000, 0x0057, 0x5f00, 0x0000, 0x0067, 0x6f00, 0x0000, 
    0x0077, 0x7f00, 0x0000, 0x1313, 0x1b00, 0x001b, 0x2323, 0x2f00, 0x002b, 
    0x2f37, 0x4300, 0x0037, 0x3b4b, 0x5707, 0x0743, 0x475f, 0x6b07, 0x0b4b, 
    0x5377, 0x830f, 0x1357, 0x5b8b, 0x9713, 0x1b5f, 0x63a3, 0xaf1f, 0x2367, 
    0x1323, 0x2f07, 0x0b17, 0x1f3b, 0x4b0f, 0x1323, 0x2b57, 0x6317, 0x1f2f, 
    0x3773, 0x7f23, 0x2b3b, 0x438f, 0x9f33, 0x334f, 0x63af, 0xbf2f, 0x2f77, 
    0x8fcf, 0xdf2b, 0x27ab, 0xcbef, 0xff1f, 0x1bf3, 0x070b, 0x1b00, 0x0013, 
    0x232b, 0x370f, 0x132b, 0x3347, 0x531b, 0x2337, 0x3f63, 0x6f2b, 0x3347, 
    0x537f, 0x8b3f, 0x475f, 0x6b9b, 0xa753, 0x5f7b, 0x87b7, 0xc36b, 0x7b93, 
    0xa3d3, 0xe38b, 0x97b3, 0x8bab, 0x9fa3, 0x977f, 0x7393, 0x8b87, 0x7b67, 
    0x5b7f, 0x776f, 0x6353, 0x4b6b, 0x5f57, 0x4b3f, 0x3757, 0x4b43, 0x372f, 
    0x2743, 0x372f, 0x231f, 0x172b, 0x231b, 0x1313, 0x0b17, 0x0f0b, 0x0707, 
    0x73bb, 0xaf9f, 0x8f6b, 0x5fa3, 0x9783, 0x7757, 0x4f8b, 0x7f6b, 0x5f4b, 
    0x4373, 0x6b53, 0x4b3b, 0x335f, 0x533f, 0x372b, 0x2347, 0x3b2b, 0x231f, 
    0x172f, 0x231b, 0x1313, 0x0b17, 0x0f0b, 0x0707, 0xc3db, 0xcbbb, 0xa7b3, 
    0xa3bf, 0xaf9b, 0x8b97, 0x87a3, 0x977b, 0x6f7b, 0x6f87, 0x7b5f, 0x5363, 
    0x576b, 0x5f47, 0x3b4b, 0x3f53, 0x4333, 0x2733, 0x2b37, 0x271f, 0x171f, 
    0x131b, 0x0f0f, 0x070b, 0x836f, 0x677b, 0x6f7b, 0x735f, 0x5767, 0x5f6b, 
    0x634f, 0x4757, 0x4f5b, 0x533f, 0x3747, 0x3f4b, 0x432f, 0x2b37, 0x2f3b, 
    0x3323, 0x1f27, 0x1f2b, 0x2317, 0x0f17, 0x131b, 0x130b, 0x070b, 0x070b, 
    0xf3ff, 0xef1b, 0x17df, 0xcbdb, 0xcb13, 0x0fb7, 0xa7bb, 0xab0f, 0x0b97, 
    0x839b, 0x8b07, 0x0773, 0x637b, 0x6b07, 0x0053, 0x475b, 0x4b00, 0x0037, 
    0x2b3b, 0x2b00, 0x001f, 0x0f1b, 0x0b00, 0x0007, 0x0000, 0x0bff, 0xef0b, 
    0x1313, 0x1bdf, 0xcf1b, 0x2323, 0x2bbf, 0xaf2b, 0x2f2f, 0x2f9f, 0x8f2f, 
    0x2f2f, 0x2f7f, 0x6f2f, 0x2f2f, 0x2b5f, 0x4f2b, 0x2323, 0x1b3f, 0x2f1b, 
    0x1313, 0x0b1f, 0x0f0b, 0x002b, 0x3b00, 0x0000, 0x074b, 0x5f00, 0x0007, 
    0x0f6f, 0x7f00, 0x0717, 0x1f93, 0xa307, 0x0b27, 0x33b7, 0xc30f, 0x1b4b, 
    0x63cf, 0xdb2b, 0x3b7f, 0x97e3, 0xe74f, 0x5fab, 0xbfef, 0xf777, 0x8bd3, 
    0x7ba7, 0xb73b, 0x379b, 0xc3c7, 0xe737, 0x57e3, 0xbf7f, 0xabff, 0xffe7, 
    0xffd7, 0x67ff, 0x0000, 0x008b, 0xb300, 0x0000, 0x00d7, 0xff00, 0x0000, 
    0xf3ff, 0xff93, 0xc7f7, 0xffff, 0x9fff, 0x535b };

SMdlPaletteColor *g_pPaletteLmp=(SMdlPaletteColor*)g_pPaletteLmpBuffer;

CMdlFileType::CMdlFileType()
{
	m_nFrames=0; 			// frames simples
	m_nSkins=0; 			// skins simples
	m_pSkinVertexes=NULL;
	m_pTriangles=NULL;
	
	int x;
	for(x=0;x<MAX_FRAMES;x++){m_pFrames[x]=NULL;}
	for(x=0;x<MAX_SKINS;x++){m_pSkins[x]=NULL;}
}

bool CMdlFileType::Open(const char *pFileName)
{
	Close();

	m_nOpenGLSkinWidth=0;
	m_nOpenGLSkinHeigth=0;

	int x=0;
	int i=1,j=1;
	long Tipo_Frame=0;
	long Tipo_Skin=0;
	
	FILE *pFile=fopen(pFileName,"rb");
	if(pFile==NULL){return false;}

	// Header
	bool bOk=(fread(&m_Header,sizeof(m_Header),1,pFile)==1);
	if(bOk)
	{
	  m_nSkins=m_Header.nSkins; 		// skins simples
	  m_nFrames=m_Header.nFrames; 		// frames simples

	  m_pSkinVertexes	=new SMdlSkinVertex[m_Header.nVertexes];
	  m_pTriangles	=new SMdlTriangle[m_Header.nTriangles];

	  while(m_nOpenGLSkinWidth<m_Header.nSkinWidth){m_nOpenGLSkinWidth=(long)pow(2.0,i);i++;}
	  while(m_nOpenGLSkinHeigth<m_Header.nSkinHeight){m_nOpenGLSkinHeigth=(long)pow(2.0,j);j++;}
	}
	  
	// Se leen los skins...
	
	for(x=0;bOk && x<m_Header.nSkins;x++)
	{
		// Si el tipo del m_pSkins es 1 se trata de un
		// grupo de skins que contiene varias imagenes
		// seguidas.

		bOk=(fread(&Tipo_Skin,sizeof(Tipo_Skin),1,pFile)==1);
		if(bOk && Tipo_Skin==0)
		{
			m_pSkins[x]=new SMdlSkin;
			m_pSkins[x]->pPixels=new unsigned char [m_Header.nSkinHeight*m_Header.nSkinWidth];
			bOk=(fread(m_pSkins[x]->pPixels,m_Header.nSkinHeight*m_Header.nSkinWidth,1,pFile)==1);
		}
	}
	
	if(bOk){bOk=(fread(m_pSkinVertexes,m_Header.nVertexes*sizeof(SMdlSkinVertex),1,pFile)==1);}
	if(bOk){bOk=(fread(m_pTriangles,m_Header.nTriangles*sizeof(SMdlTriangle),1,pFile)==1);}
	
	// Se leen los Frames...
	
	for(x=0;bOk && x<m_Header.nFrames;x++)
	{
		// Si el tipo del frame es 1 se trata de un
		// grupo de skins que contiene varias imagenes
		// seguidas.

		bOk=(fread(&Tipo_Frame,sizeof(Tipo_Frame),1,pFile)==1);
		if(bOk && Tipo_Frame==0)
		{
			m_pFrames[x]=new SMdlFrame;
			m_pFrames[x]->pnVertexIndexes=new SMdlCompressedVertex[m_Header.nVertexes];
			if(bOk){bOk=(fread(m_pFrames[x],sizeof(SMdlFrame)-4,1,pFile)==1);}
			if(bOk){bOk=(fread(m_pFrames[x]->pnVertexIndexes,sizeof(SMdlCompressedVertex)*m_Header.nVertexes,1,pFile)==1);}
		}
	}
	
	if(pFile!=NULL){fclose(pFile);pFile=NULL;}
	
	return bOk;
}

void CMdlFileType::Close()
{
	int x;
	
	if(m_pTriangles!=NULL){delete [] m_pTriangles;m_pTriangles=NULL;}
	if(m_pSkinVertexes!=NULL){delete [] m_pSkinVertexes;m_pSkinVertexes=NULL;}
	
	for(x=0;x<MAX_SKINS;x++){if(m_pSkins[x]!=NULL){delete m_pSkins[x];m_pSkins[x]=NULL;}}
	for(x=0;x<MAX_FRAMES;x++){if(m_pFrames[x]!=NULL){delete m_pFrames[x];m_pFrames[x]=NULL;}}

	m_nFrames=0;
	m_nSkins=0;
	m_pSkinVertexes=NULL;
	m_pTriangles=NULL;
}

CMdlFileType::~CMdlFileType()
{
	Close();
}


CPolyhedron *CMdlFileType::GetPolyhedron(int frame,int skin)
{
	CPolyhedron *pPolyhedron=new CPolyhedron;
	CVector vTriangleVertexes[3];
	CVector vTextCoord[3];
	
	for(int n=0;n<m_Header.nTriangles;n++)
	{
		vTriangleVertexes[0].c[0]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[0]].x)*((double)m_Header.pfScale[0])+(double)m_Header.pfOrigin[0];
		vTriangleVertexes[0].c[1]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[0]].z)*((double)m_Header.pfScale[2])+(double)m_Header.pfOrigin[1];
		vTriangleVertexes[0].c[2]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[0]].y)*((double)m_Header.pfScale[1])+(double)m_Header.pfOrigin[2];
		vTriangleVertexes[1].c[0]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[1]].x)*((double)m_Header.pfScale[0])+(double)m_Header.pfOrigin[0];
		vTriangleVertexes[1].c[1]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[1]].z)*((double)m_Header.pfScale[2])+(double)m_Header.pfOrigin[1];
		vTriangleVertexes[1].c[2]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[1]].y)*((double)m_Header.pfScale[1])+(double)m_Header.pfOrigin[2];
		vTriangleVertexes[2].c[0]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[2]].x)*((double)m_Header.pfScale[0])+(double)m_Header.pfOrigin[0];
		vTriangleVertexes[2].c[1]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[2]].z)*((double)m_Header.pfScale[2])+(double)m_Header.pfOrigin[1];
		vTriangleVertexes[2].c[2]=((double)m_pFrames[frame]->pnVertexIndexes[m_pTriangles[n].pnVertexIndexes[2]].y)*((double)m_Header.pfScale[1])+(double)m_Header.pfOrigin[2];

		vTextCoord[0].c[0]=((double)m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[2]].x)/((double)m_Header.nSkinWidth);
		vTextCoord[0].c[1]=((double)m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[2]].y)/((double)m_Header.nSkinHeight);
		vTextCoord[1].c[0]=((double)m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[1]].x)/((double)m_Header.nSkinWidth);
		vTextCoord[1].c[1]=((double)m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[1]].y)/((double)m_Header.nSkinHeight);
		vTextCoord[2].c[0]=((double)m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[0]].x)/((double)m_Header.nSkinWidth);
		vTextCoord[2].c[1]=((double)m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[0]].y)/((double)m_Header.nSkinHeight);
		
		if(m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[0]].nBack==0x20 && !m_pTriangles[n].nFront){vTextCoord[2].c[0]+=0.5;}
		if(m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[1]].nBack==0x20 && !m_pTriangles[n].nFront){vTextCoord[1].c[0]+=0.5;}
		if(m_pSkinVertexes[m_pTriangles[n].pnVertexIndexes[2]].nBack==0x20 && !m_pTriangles[n].nFront){vTextCoord[0].c[0]+=0.5;}

		// A veces se cargan poligonos no validos (como por ejemplo 3 puntos en linea)
		// ha pasado con el poligono 1 del frame 0 del modelo soldier.mdl
		// Por lo tanto hay que comprbar la valided del poligono antes de meterlo en el poliedro
		// la comprobacion que se hace es la de calcular el std::vector normal al poligono, no se introduce si alguna
		// de sus componentes es NaN o las tres son cero.

		CVector t1;
		CVector t2;
		CVector t3;
		t1=vTriangleVertexes[1]-vTriangleVertexes[0];
		t2=vTriangleVertexes[2]-vTriangleVertexes[0];
		t3=t1^t2;
		if(t3.c[0]==t3.c[0] && t3.c[1]==t3.c[1] && t3.c[2]==t3.c[2] && 
			(t3.c[0]!=0 || t3.c[1]!=0 || t3.c[2]!=0))
		{
			CPolygon *pPolygon=new CPolygon(3,vTriangleVertexes);
			pPolyhedron->m_vPolygons.push_back(pPolygon);
		}
	}
	return pPolyhedron;
}

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

CMapFileBrush::CMapFileBrush()
{
}

CMapFileBrush::~CMapFileBrush()
{
	for(unsigned int x=0;x<m_vPolygons.size();x++)
	{
		CMapFilePolygon *pPolygon=m_vPolygons[x];
		delete pPolygon;
	}
}

#define MAX_TEXT_FILE_LENGHT 1024*1024
CMapFilePolygon::CMapFilePolygon(CPolygon &polygon)
{
	m_sTextName[0]=0;
	*(CMaterialPolygon *)this=polygon;
}

CMapFilePolygon::~CMapFilePolygon()
{
}

CMapFileType::CMapFileType()
{
}

CMapFileType::~CMapFileType()
{
	Close();
}

struct SMapFileVertexTextureInfo
{
	char   sFileName[MAX_PATH];
	double s;
	double t;
};

CMapFileBrush *CMapFileType::ReadBrush(char *pBuffer,unsigned int *pOffset,unsigned int fileLen,double size)
{
	std::vector<CVector>					vVectors;
	std::vector<SMapFileVertexTextureInfo>	vTextureInfo;
	int		nPlanes=0;
	CPlane *pPlanes=NULL;

	while((*pOffset)!=(fileLen-1))
	{
		// vector start
		if(pBuffer[(*pOffset)]=='(')
		{
			char *pTempBuffer=NULL;

			(*pOffset)++;
			CVector v;
			for(int y=0;y<3;y++)
			{
				// 3 coordinates
				for(int x=0;x<3;x++)
				{
					pTempBuffer=strtok(x==0 && y==0?pBuffer+*pOffset:NULL,"(, )");
					if(pTempBuffer)
					{
						v.c[x]=atof(pTempBuffer);
						(*pOffset)=pTempBuffer-pBuffer+strlen(pTempBuffer)+1;
					}
				}
				// Coordinate system conversion
				double temp=v.c[2];
				v.c[2]=-v.c[1];
				v.c[1]=temp;
				vVectors.push_back(v);
			}
			double dTextAngle=0.0;
			CVector vTextScale;

			SMapFileVertexTextureInfo info;
			pTempBuffer=strtok(pBuffer+*pOffset,"(, )");
			if(pTempBuffer){strcpy(info.sFileName,pTempBuffer);(*pOffset)=pTempBuffer-pBuffer+strlen(pTempBuffer)+1;}
			pTempBuffer=strtok(pBuffer+*pOffset,"(, )");
			if(pTempBuffer){info.s=atof(pTempBuffer);(*pOffset)=pTempBuffer-pBuffer+strlen(pTempBuffer)+1;}
			pTempBuffer=strtok(pBuffer+*pOffset,"(, )");
			if(pTempBuffer){info.t=atof(pTempBuffer);(*pOffset)=pTempBuffer-pBuffer+strlen(pTempBuffer)+1;}
			pTempBuffer=strtok(pBuffer+*pOffset,"(, )");
			if(pTempBuffer){dTextAngle=atof(pTempBuffer);(*pOffset)=pTempBuffer-pBuffer+strlen(pTempBuffer)+1;}
			pTempBuffer=strtok(pBuffer+*pOffset,"(, )");
			if(pTempBuffer){vTextScale.c[0]=atof(pTempBuffer);(*pOffset)=pTempBuffer-pBuffer+strlen(pTempBuffer)+1;}
			pTempBuffer=strtok(pBuffer+*pOffset,"(, )");
			if(pTempBuffer){vTextScale.c[1]=atof(pTempBuffer);(*pOffset)=pTempBuffer-pBuffer+strlen(pTempBuffer)+1;}
			vTextureInfo.push_back(info);
/*
			std::string path;
			path+="Textures\\";
			path+=sTextureName;
			path+=".bmp";
			CMRImage image;
			image.LoadFromFile(path.c_str());*/
			continue;
		}
		// entity end
		else if(pBuffer[(*pOffset)]=='}')
		{
			(*pOffset)++;
			break;
		}
		(*pOffset)++;
	}

	if(vVectors.size()>=3)
	{
		nPlanes=vVectors.size()/3;
		pPlanes=new CPlane[nPlanes];
		for(unsigned x=0;x<vVectors.size();x+=3)
		{
			CVector vectors[3]={vVectors[x],vVectors[x+1],vVectors[x+2]};
			pPlanes[x/3]=CPlane(vectors[0],vectors[1],vectors[2]);
			CVector newPoint=((CVector)pPlanes[x/3])*(pPlanes[x/3].d+size);
			pPlanes[x/3].d=newPoint*(CVector)pPlanes[x/3];
		}
	}

	CMapFileBrush	*pBrush=new CMapFileBrush;
	CPolyhedron		*pPolyhedron=PolyhedronFromConvexRegion(nPlanes,pPlanes);
	if(pPolyhedron)
	{
		for(unsigned int x=0;x<pPolyhedron->m_vPolygons.size();x++)
		{
			CPolygon		*pPolygon=pPolyhedron->m_vPolygons[x];
			CMapFilePolygon	*pMapPolygon=new CMapFilePolygon(*pPolygon);
			for(int z=0;z<nPlanes;z++)
			{
				if(pMapPolygon->m_Plane==pPlanes[z])
				{
					strcpy(pMapPolygon->m_sTextName,vTextureInfo[z].sFileName);
					for(unsigned int y=0;y<pMapPolygon->m_nVertexes;y++)
					{
						//CVector temp=pMapPolygon->m_Plane*pMapPolygon->m_pVertexes[y];
						CVector dist=(pMapPolygon->m_pVertexes[y]-pMapPolygon->m_pVertexes[0]);
						pMapPolygon->m_pTextureCoords[y].c[0]=vTextureInfo[z].s+dist.c[0];
						pMapPolygon->m_pTextureCoords[y].c[1]=vTextureInfo[z].t+dist.c[1];
					}
				}
			}
			pBrush->m_vPolygons.push_back(pMapPolygon);
		}
		delete pPolyhedron;
	}


	if(pPlanes){delete [] pPlanes;}
	return pBrush;
}

void CMapFileType::SkipCommentsStringsAndSpaces(char *pBuffer,unsigned int *pOffset,unsigned int bufLen)
{
	bool inLineComment=false;
	bool inParagraphComment=false;
	bool inString=false;
	unsigned int x;

	for(x=(*pOffset);x<bufLen;x++)
	{
		if(inLineComment)
		{
			if(x!=0 && pBuffer[x]=='\n')
			{inLineComment=false;}
		}
		else
			if(inParagraphComment)
			{
				if(x!=0 && pBuffer[x-1]=='*' && pBuffer[x]=='/')
				{inParagraphComment=false;}
			}
			else
				if(inString)
				{
					// this -> \" is a colon character inside a string.
					if(x==0 || pBuffer[x-1]!='\\' || pBuffer[x]!='"')
					{if(pBuffer[x]=='"'){inString=false;}}
				}
				else
				{
					if(x+1<bufLen)
					{
						if(pBuffer[x]=='/' && pBuffer[x+1]=='*'){inParagraphComment =true;}
						if(pBuffer[x]=='/' && pBuffer[x+1]=='/'){inLineComment =true;}
					}	
					if(pBuffer[x]=='"'){inString=true;}

					if(!inParagraphComment && !inLineComment && !inString)
					{
						if(pBuffer[x]!=' ' && pBuffer[x]!='\n' && pBuffer[x]!='\r'  && pBuffer[x]!='\t'){break;}
					}
				}
	}
	(*pOffset)=x;
}

bool CMapFileType::Open(const char *pMap)
{
	std::list<CPolyhedron *>::iterator i;
	char *pBuffer=new char [MAX_TEXT_FILE_LENGHT];
	unsigned int fileLen=0;
	unsigned int offset=0;
	FILE *pFile=fopen(pMap,"rb");
	if(pFile!=NULL)
	{
		fileLen=fread(pBuffer,1,MAX_TEXT_FILE_LENGHT,pFile);
		fclose(pFile);
		pFile=NULL;
	}
	if(!fileLen){return false;}

	SkipCommentsStringsAndSpaces(pBuffer,&offset,fileLen);
	while(offset!=(fileLen-1))
	{
		// For all entities
		while(offset!=(fileLen-1))
		{
			// Check for Entity start
			if(pBuffer[offset]=='{')
			{
				offset++;
				while(offset!=(fileLen-1))
				{
					// brush start
					if(pBuffer[offset]=='{')
					{
						offset++;
						CMapFileBrush *pBrush=ReadBrush(pBuffer,&offset,fileLen,0.0);
						if(pBrush)
						{
							m_lBrushes.push_back(pBrush);
						}
						continue;
					}
					// entity end
					else if(pBuffer[offset]=='}')
					{
						offset++;
						break;
					}
					offset++;
				}

				continue;
			}
			offset++;
		}
	}
	delete [] pBuffer;
	return true;
}

void CMapFileType::Close()
{
	std::list<CMapFileBrush *>::iterator i;
	for(i=m_lBrushes.begin();i!=m_lBrushes.end();i++)
	{
		CMapFileBrush *pBrush=*i;
		delete pBrush;
	}
	m_lBrushes.clear();
}

