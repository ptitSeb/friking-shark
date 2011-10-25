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


// ASEFileType.cpp: implementation of the CASEFileType class.
//
//////////////////////////////////////////////////////////////////////


#include "./StdAfx.h"
#include "VectorLib.h"
#include "3DSTypes.h"
#include "3DSFiles.h"
#include "ASEFiles.h"
#include "GCMFiles.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


using namespace std;

#define ASE_FILE_DELIMITER " \t:\r\n\""
#define SKIP_ASE_TOKEN() strtok(NULL,ASE_FILE_DELIMITER)


#define ASE_OBJECT					1
#define ASE_OBJECT_MESH				2
#define ASE_TRANSFORM_INFO	3
#define ASE_INHERIT_POS				4
#define ASE_INHERIT_ROT				5
#define ASE_INHERIT_SCL				6
#define ASE_TM_ROW0					7
#define ASE_TM_ROW1					8
#define ASE_TM_ROW2					9
#define ASE_TM_ROW3					10
#define ASE_TM_POS					11
#define ASE_TM_ROTAXIS				12
#define ASE_TM_ROTANGLE				13
#define ASE_TM_SCALE				14
#define ASE_TM_SCALEAXIS			15
#define ASE_TM_SCALEAXISANG			16
#define ASE_MESH_NUMVERTEX			17
#define ASE_MESH_NUMFACES			18
#define ASE_MESH_VERTEX				19
#define ASE_MESH_TVERT				20
#define ASE_MESH_FACE				21
#define ASE_MATERIAL				22
#define ASE_MATERIAL_NAME			23
#define ASE_MATERIAL_CLASS			24
#define ASE_MATERIAL_AMBIENT		25
#define ASE_MATERIAL_DIFFUSE		26
#define ASE_MATERIAL_SPECULAR		27
#define ASE_MATERIAL_SHINE			28
#define ASE_MATERIAL_SHINESTRENGTH	29
#define ASE_MATERIAL_TRANSPARENCY	30
#define ASE_MATERIAL_REF			31
#define ASE_MATERIAL_LIST			32
#define ASE_MESH_VERTEX_LIST		33
#define ASE_MESH_FACE_LIST			34
#define ASE_SUBMATERIAL				35
#define ASE_MESH_MTLID				36
#define ASE_MAP_DIFFUSE				37
#define ASE_BITMAP					38
#define ASE_MESH_TVERTLIST			39
#define ASE_LIGHTOBJECT				40
#define ASE_LIGHT_TYPE 				41
#define ASE_LIGHT_USELIGHT			42
#define ASE_LIGHT_SETTINGS			43
#define ASE_LIGHT_COLOR				44
#define ASE_LIGHT_INTENS			45
#define ASE_LIGHT_HOTSPOT			46
#define ASE_LIGHT_FALLOFF			47
#define ASE_LIGHT_ATTNSTART			48
#define ASE_LIGHT_ATTNEND			49
#define ASE_MESH_NUMTVERTEX			50
#define ASE_MESH_NUMTVFACES			51
#define ASE_MESH_TFACELIST			52
#define ASE_MESH_TFACE				53
#define ASE_LIGHT_TDIST				54
#define ASE_UVW_U_OFFSET			55
#define ASE_UVW_V_OFFSET			56
#define ASE_UVW_U_TILING			57
#define ASE_UVW_V_TILING			58
#define ASE_UVW_ANGLE				59
#define ASE_MESH_NORMALS			60
#define ASE_MESH_VERTEXNORMAL		61
#define ASE_WIREFRAME_COLOR			62
#define ASE_MAP_OPACITY				63
#define ASE_MATERIAL_TWOSIDED		64
#define ASE_MESH_NUMCVERTEX			65
#define ASE_MESH_NUMCVFACES			66
#define ASE_MESH_CFACELIST			67
#define ASE_MESH_CFACE				68
#define ASE_MESH_CVERTLIST			69
#define ASE_MESH_VERTCOL			70
#define	ASE_SCENE					71
#define	ASE_SCENE_FIRSTFRAME		72
#define	ASE_SCENE_LASTFRAME			73
#define	ASE_SCENE_FRAMESPEED		74
#define	ASE_SCENE_TICKSPERFRAME		75
#define	ASE_TIMEVALUE				76
#define	ASE_MESH_ANIMATION			77
#define ASE_MESH_SMOOTHING			78
#define ASE_MESH_FACENORMAL			79
#define ASE_MAP_BUMP				80


char *FindNodeEnd(char *pBuffer)
{
	int index=0;
	while(pBuffer[index]!='}' && pBuffer[index]!=0)
	{
		if(pBuffer[index]=='{')
		{
			char *pResult=FindNodeEnd(pBuffer+index+1);
			index=pResult-pBuffer;
		}
		if(pBuffer[index]=='"')
		{
			do
			{
				index++;
			}
			while(pBuffer[index]!='"' && pBuffer[index]!=0);
		}
		index++;
	}
	if(pBuffer[index]=='}'){index++;}
	return pBuffer+index;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CASEFileType::CASEFileType()
{
	m_pFile=NULL;
	m_pBuffer=NULL;
}

unsigned short			CASEFileType::ReadWord()  {unsigned short value=0;char *pBuf=strtok(NULL,ASE_FILE_DELIMITER);value=atoi(pBuf);return value;}
unsigned int			CASEFileType::ReadDWord() {unsigned int value=0;char *pBuf=strtok(NULL,ASE_FILE_DELIMITER);value=atol(pBuf);return value;}
float			CASEFileType::ReadFloat() {float value=0;char *pBuf=strtok(NULL,ASE_FILE_DELIMITER);value=(float)atof(pBuf);return value;}
unsigned char	CASEFileType::ReadByte()  {unsigned char value=0;char *pBuf=strtok(NULL,ASE_FILE_DELIMITER);value=atoi(pBuf);return value;}
CVector			CASEFileType::ReadVector(){CVector v;v.c[0]=ReadFloat();v.c[1]=ReadFloat();v.c[2]=ReadFloat();return v;}
string			CASEFileType::ReadString(){string temp;char *pBuf=strtok(NULL,"\"\t\r\n");temp=pBuf;return temp;}

bool CASEFileType::Open(const char *sFileName)
{
	unsigned int dwFileLength=0;

	if ((m_pFile=fopen (sFileName, "rb"))== NULL) return false; //Open the file

	fseek(m_pFile,0,SEEK_END);
    dwFileLength=ftell(m_pFile);
	fseek(m_pFile,0,SEEK_SET);
	
	if(!dwFileLength){return false;}


	bool				bBuildAnimation=false;
	S3DSObject			*pObject=NULL;
	S3DSFrame			*pFrame=NULL;
	S3DSLight			*pLight=NULL;
	S3DSCamera			*pCamera=NULL;
	S3DSMaterial		*pMaterial=NULL;
	char				*pToken=NULL;
	unsigned int		dwCurrentMaterialId=0;
	unsigned int		dwFaceIndex=0;
	unsigned int		nTempNormalFaceIndex=0;
					
	CVector				*pCurrentOrigin=NULL;
	CMatrix				*pTransformMatrix=NULL;
	CVector				*pAxises=NULL;
	CVector				*pRotationAxis=NULL;
	CVector				*pScale=NULL;
	float				*pRotationAngle=NULL;
	unsigned int				dwCurrentBitmap=0;
	bool				bReadingAnimation=false;
	unsigned int x;
	int y;

	m_pBuffer=new char [dwFileLength+1];
	if(fread(m_pBuffer,dwFileLength,1,m_pFile)!=1){Close();return false;}
	m_pBuffer[dwFileLength]=0;

	map<string,unsigned int> mKeyNames;
	mKeyNames["*GEOMOBJECT"]			=ASE_OBJECT;
	mKeyNames["*MESH"]					=ASE_OBJECT_MESH;
	mKeyNames["*NODE_TM"]				=ASE_TRANSFORM_INFO;
	mKeyNames["*TM_ROW0"]				=ASE_TM_ROW0;
	mKeyNames["*TM_ROW1"]				=ASE_TM_ROW1;
	mKeyNames["*TM_ROW2"]				=ASE_TM_ROW2;
	mKeyNames["*TM_ROW3"]				=ASE_TM_ROW3;
	mKeyNames["*TM_POS"]				=ASE_TM_POS;
	mKeyNames["*TM_ROTAXIS"]			=ASE_TM_ROTAXIS;
	mKeyNames["*TM_ROTANGLE"]			=ASE_TM_ROTANGLE;
	mKeyNames["*TM_SCALE"]				=ASE_TM_SCALE;
	mKeyNames["*TM_SCALEAXIS"]			=ASE_TM_SCALEAXIS;
	mKeyNames["*TM_SCALEAXISANG"]		=ASE_TM_SCALEAXISANG;
	mKeyNames["*MESH_NUMVERTEX"]		=ASE_MESH_NUMVERTEX;
	mKeyNames["*MESH_NUMFACES"]			=ASE_MESH_NUMFACES;
	mKeyNames["*MESH_VERTEX"]			=ASE_MESH_VERTEX;
	mKeyNames["*MESH_TVERT"]			=ASE_MESH_TVERT;
	mKeyNames["*MESH_FACE"]				=ASE_MESH_FACE;
	mKeyNames["*MESH_MTLID"]			=ASE_MESH_MTLID;
	mKeyNames["*MESH_SMOOTHING"]		=ASE_MESH_SMOOTHING;
	mKeyNames["*MESH_FACENORMAL"]		=ASE_MESH_FACENORMAL;
	mKeyNames["*MESH_FACE_LIST"]		=ASE_MESH_FACE_LIST;
	mKeyNames["*MESH_VERTEX_LIST"]		=ASE_MESH_VERTEX_LIST;
	mKeyNames["*MATERIAL"]				=ASE_MATERIAL;
	mKeyNames["*MATERIAL_LIST"]			=ASE_MATERIAL_LIST;
	mKeyNames["*MATERIAL_NAME"]			=ASE_MATERIAL_NAME;
	mKeyNames["*MATERIAL_CLASS"]		=ASE_MATERIAL_CLASS;
	mKeyNames["*MATERIAL_AMBIENT"]		=ASE_MATERIAL_AMBIENT;
	mKeyNames["*MATERIAL_DIFFUSE"]		=ASE_MATERIAL_DIFFUSE;
	mKeyNames["*MATERIAL_SPECULAR"]		=ASE_MATERIAL_SPECULAR;
	mKeyNames["*MATERIAL_SHINE"]		=ASE_MATERIAL_SHINE;
	mKeyNames["*MATERIAL_SHINESTRENGTH"]=ASE_MATERIAL_SHINESTRENGTH;
	mKeyNames["*MATERIAL_TRANSPARENCY"]	=ASE_MATERIAL_TRANSPARENCY;
	mKeyNames["*MATERIAL_REF"]			=ASE_MATERIAL_REF;
	mKeyNames["*SUBMATERIAL"]			=ASE_SUBMATERIAL;
	mKeyNames["*MAP_DIFFUSE"]			=ASE_MAP_DIFFUSE;
	mKeyNames["*MAP_BUMP"]				=ASE_MAP_BUMP;
	mKeyNames["*MAP_OPACITY"]			=ASE_MAP_OPACITY;
	mKeyNames["*BITMAP"]				=ASE_BITMAP;
	mKeyNames["*MESH_TVERTLIST"]		=ASE_MESH_TVERTLIST;
	mKeyNames["*LIGHTOBJECT"]			=ASE_LIGHTOBJECT;
	mKeyNames["*LIGHT_TYPE"]			=ASE_LIGHT_TYPE;
	mKeyNames["*LIGHT_USELIGHT"]		=ASE_LIGHT_USELIGHT;
	mKeyNames["*LIGHT_SETTINGS"]		=ASE_LIGHT_SETTINGS;
	mKeyNames["*LIGHT_COLOR"]			=ASE_LIGHT_COLOR;
	mKeyNames["*LIGHT_INTENS"]			=ASE_LIGHT_INTENS;
	mKeyNames["*LIGHT_HOTSPOT"]			=ASE_LIGHT_HOTSPOT;
	mKeyNames["*LIGHT_FALLOFF"]			=ASE_LIGHT_FALLOFF;
	mKeyNames["*LIGHT_ATTNSTART"]		=ASE_LIGHT_ATTNSTART;
	mKeyNames["*LIGHT_ATTNEND"]			=ASE_LIGHT_ATTNEND;
	mKeyNames["*MESH_NUMTVERTEX"]		=ASE_MESH_NUMTVERTEX;
	mKeyNames["*MESH_NUMTVFACES"]		=ASE_MESH_NUMTVFACES;
	mKeyNames["*MESH_TFACELIST"]		=ASE_MESH_TFACELIST;
	mKeyNames["*MESH_TFACE"]			=ASE_MESH_TFACE;
	mKeyNames["*MESH_NUMCVERTEX"]		=ASE_MESH_NUMCVERTEX;
	mKeyNames["*MESH_NUMCVFACES"]		=ASE_MESH_NUMCVFACES;
	mKeyNames["*MESH_NUMCFACES"]		=ASE_MESH_NUMCVFACES;// For Blender ASE exporter.
	mKeyNames["*MESH_CFACELIST"]		=ASE_MESH_CFACELIST;
	mKeyNames["*MESH_CFACE"]			=ASE_MESH_CFACE;
	mKeyNames["*MESH_CVERTLIST"]		=ASE_MESH_CVERTLIST;
	mKeyNames["*MESH_VERTCOL"]			=ASE_MESH_VERTCOL;
	mKeyNames["*LIGHT_TDIST"]			=ASE_LIGHT_TDIST;
	mKeyNames["*UVW_U_OFFSET"]			=ASE_UVW_U_OFFSET;
	mKeyNames["*UVW_V_OFFSET"]			=ASE_UVW_V_OFFSET;
	mKeyNames["*UVW_U_TILING"]			=ASE_UVW_U_TILING;
	mKeyNames["*UVW_V_TILING"]			=ASE_UVW_V_TILING;
	mKeyNames["*UVW_ANGLE"]	  			=ASE_UVW_ANGLE;
	mKeyNames["*MESH_NORMALS"]	  		=ASE_MESH_NORMALS;
	mKeyNames["*MESH_VERTEXNORMAL"]	  	=ASE_MESH_VERTEXNORMAL;
	mKeyNames["*WIREFRAME_COLOR"]	  	=ASE_WIREFRAME_COLOR;
	mKeyNames["*MATERIAL_TWOSIDED"]	  	=ASE_MATERIAL_TWOSIDED;
	mKeyNames["*SCENE"]	  				=ASE_SCENE;
	mKeyNames["*SCENE_FIRSTFRAME"]	  	=ASE_SCENE_FIRSTFRAME;
	mKeyNames["*SCENE_LASTFRAME"]	  	=ASE_SCENE_LASTFRAME;
	mKeyNames["*SCENE_FRAMESPEED"]	  	=ASE_SCENE_FRAMESPEED;
	mKeyNames["*SCENE_TICKSPERFRAME"]	=ASE_SCENE_TICKSPERFRAME;
	mKeyNames["*MESH_ANIMATION"]	  	=ASE_MESH_ANIMATION;
	mKeyNames["*TIMEVALUE"]	  			=ASE_TIMEVALUE;

	// usando pSearchBuffer podemos poner pToken a cero para seguir la busqueda en otro punto
	char *pSearchBuffer=m_pBuffer; 
	while((pToken=strtok(pToken?NULL:pSearchBuffer,ASE_FILE_DELIMITER)))
	{
		map<string,unsigned int>::iterator i=mKeyNames.find(pToken);
		if(i==mKeyNames.end())
		{
			//_MRT("Unknown key name %s",pToken);
			// si lo que hemos encontrado es una llave es un nodo que no se sabe leer, esquivamos todo 
			// hasta el fin del nodo.
			if(strcmp(pToken,"{")==0)
			{
				pSearchBuffer=FindNodeEnd(pToken+strlen(pToken)+1);
				pToken=NULL;
			}
			continue;
		}
		unsigned int dwKeyId=i->second;
		switch(dwKeyId)
		{
			// Escena

			case ASE_SCENE: 				
				{SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_SCENE_FIRSTFRAME : 	
				{m_Scene.dFirstFrame=ReadFloat();}break;
			case ASE_SCENE_LASTFRAME : 		
				{m_Scene.dLastFrame=ReadFloat();}break;
			case ASE_SCENE_FRAMESPEED: 		
				{m_Scene.dFrameSpeed=ReadFloat();}break;
			case ASE_SCENE_TICKSPERFRAME : 	
				{m_Scene.dTicksPerFrame=ReadFloat();}break;

			// Objetos

			case ASE_OBJECT: 
				{
					SKIP_ASE_TOKEN(); // Skip '{'
					SKIP_ASE_TOKEN(); // Skip *NODE_NAME
					bReadingAnimation=false;
					pLight=NULL;
					pCamera=NULL;
					pObject=new S3DSObject;
					pFrame=&pObject->baseFrame;
					pTransformMatrix=&pObject->mTransformMatrix;
					pAxises=pObject->vAxises;
					pCurrentOrigin=&pObject->vOrigin;
					pRotationAxis=&pObject->vFirstFrameRotationAxis;
					pRotationAngle=&pObject->fFirstFrameRotationAngle;
					pScale=&pObject->vFirstFrameScale;
					m_vObjects.push_back(pObject);
					strcpy(pObject->sName,ReadString().c_str());
				}
			break;
			case ASE_MESH_ANIMATION:		{bBuildAnimation=true;bReadingAnimation=true;SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_TIMEVALUE:				{pFrame->dTimeStamp=ReadFloat();}break;
			case ASE_OBJECT_MESH:			
				{
					SKIP_ASE_TOKEN(); /* Skip '{'*/
					if(bReadingAnimation)
					{
						pFrame=new S3DSFrame;
						pObject->vFrameInstances.push_back(pFrame);
					}
				}
				break;
			case ASE_TRANSFORM_INFO: 
				{
					SKIP_ASE_TOKEN(); /* Skip '{'*/

					if(pLight)
					{
						SKIP_ASE_TOKEN(); /* Skip *NODE_NAME*/
						string targetName=ReadString();
						if(strcmp(targetName.c_str(),pLight->sName)!=0)
						{
							pLight->bHasTarget=true;
							pCurrentOrigin=&pLight->vSpotTarget;
						}

					}
				}
				break;
			case ASE_TM_ROW0:
			case ASE_TM_ROW1:
			case ASE_TM_ROW2:
			case ASE_TM_ROW3:
				{
					CVector vTemp=ReadVector();
					pTransformMatrix->e[dwKeyId-ASE_TM_ROW0][0]=vTemp.c[0];
					pTransformMatrix->e[dwKeyId-ASE_TM_ROW0][1]=vTemp.c[1];
					pTransformMatrix->e[dwKeyId-ASE_TM_ROW0][2]=vTemp.c[2];
					if(dwKeyId!=ASE_TM_ROW3){pAxises[dwKeyId-ASE_TM_ROW0]=vTemp;}
				}
			break;
			case ASE_TM_POS:
				{
					*pCurrentOrigin=ReadVector();
				}
				break;
			case ASE_TM_ROTAXIS:
				{
					*pRotationAxis=ReadVector();
				}
				break;
			case ASE_TM_ROTANGLE:
				{
					*pRotationAngle=ReadFloat();
				}
				break;
			case ASE_TM_SCALE:
				{
					*pScale=ReadVector();
				}
				break;
			case ASE_MATERIAL_REF:
				{pObject->dwMaterialId=ReadDWord();}break;
			case ASE_MESH_NUMVERTEX: 
				{
					pFrame->nVertexes = ReadDWord();
					pFrame->pVertexes = new CVector[pFrame->nVertexes];
				}
			break;	
			case ASE_MESH_VERTEX_LIST:	{SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_MESH_VERTEX: 
				{
					unsigned int iVertexIndex=ReadDWord();
					pFrame->pVertexes[iVertexIndex]=ReadVector();
				}
			break;	
			case ASE_MESH_NUMFACES:
				{
					pFrame->nFaces = ReadDWord();
					pFrame->pFaces=new int[pFrame->nFaces*3];
					pFrame->pbFaceSmooth=new bool[pFrame->nFaces];
					pFrame->pEdges=new bool [pFrame->nFaces*3];
					pFrame->pFaceSubMaterials=new unsigned int[pFrame->nFaces];
					memset(pFrame->pbFaceSmooth,0,sizeof(bool)*pFrame->nFaces);
					memset(pFrame->pFaces,0,sizeof(int)*pFrame->nFaces*3);
					memset(pFrame->pEdges,0,sizeof(bool)*pFrame->nFaces*3);
					memset(pFrame->pFaceSubMaterials,0,sizeof(unsigned int)*pFrame->nFaces);
					pFrame->pEdges[0]=1;
					pFrame->pEdges[1]=1;
					pFrame->pEdges[2]=1;
				}
			break;

			case ASE_MESH_NORMALS:	
				{
					SKIP_ASE_TOKEN(); /* Skip '{'*/
					pFrame->pFaceNormals=new CVector[pFrame->nFaces];
					pFrame->pVertexNormals = new CVector[pFrame->nFaces*3];
				}
			break;
			case ASE_MESH_VERTEXNORMAL:
				{
					unsigned int dwVertexIndex=ReadDWord();
					for(int x=0;x<3;x++)
					{
						int nFaceVertexPos=nTempNormalFaceIndex*3+x;
						if(dwVertexIndex==(unsigned int)pFrame->pFaces[nFaceVertexPos])
						{
							pFrame->pVertexNormals[nFaceVertexPos]=ReadVector();
							break;
						}
					}
				}
			break;
			case ASE_MESH_FACENORMAL:
				{
					nTempNormalFaceIndex=ReadDWord();
					pFrame->pFaceNormals[nTempNormalFaceIndex]=ReadVector();
				}
			break;
			case ASE_MESH_FACE_LIST:	{SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_MESH_FACE:
				{
					dwFaceIndex=ReadDWord();
					SKIP_ASE_TOKEN(); /* Skip ':     A:'*/
					pFrame->pFaces[dwFaceIndex*3]=ReadDWord();
					SKIP_ASE_TOKEN(); /* Skip ':     B:'*/
					pFrame->pFaces[dwFaceIndex*3+1]=ReadDWord();
					SKIP_ASE_TOKEN(); /* Skip ':     C:'*/
					pFrame->pFaces[dwFaceIndex*3+2]=ReadDWord();
					SKIP_ASE_TOKEN(); /* Skip ':     AB:'*/
					pFrame->pEdges[dwFaceIndex*3]=(bool)(ReadDWord()!=0);
					SKIP_ASE_TOKEN(); /* Skip ':     BC:'*/
					pFrame->pEdges[dwFaceIndex*3+1]=(bool)(ReadDWord()!=0);
					SKIP_ASE_TOKEN(); /* Skip ':     CA:'*/
					pFrame->pEdges[dwFaceIndex*3+2]=(bool)(ReadDWord()!=0);
				}	
			break;
			case ASE_MESH_NUMTVERTEX:
				{
					pFrame->nTextVertexes = ReadDWord();
					pFrame->pTextVertexes =new CVector[pFrame->nTextVertexes];
				}
			break;
			case ASE_MESH_TVERTLIST:	{SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_MESH_TVERT: 
				{
					unsigned int iTextVertexIndex=ReadDWord();
					pFrame->pTextVertexes[iTextVertexIndex]=ReadVector();
				}
			break;	
			case ASE_MESH_NUMTVFACES:
				{
					pFrame->nTextFaces = ReadDWord();
					pFrame->pTextFaces =new S3DSTextureFace[pFrame->nTextFaces];
				}
			break;
			case ASE_MESH_TFACELIST:	{SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_MESH_TFACE:
				{
					unsigned int dwTextFaceIndex=ReadDWord();
					pFrame->pTextFaces[dwTextFaceIndex].nFaceIndex=dwTextFaceIndex;
					pFrame->pTextFaces[dwTextFaceIndex].pTextVertexes[0]=ReadDWord();
					pFrame->pTextFaces[dwTextFaceIndex].pTextVertexes[1]=ReadDWord();
					pFrame->pTextFaces[dwTextFaceIndex].pTextVertexes[2]=ReadDWord();
				}
			break;



			case ASE_MESH_NUMCVERTEX:
				{
					pFrame->nColorVertexes = ReadDWord();
					if(pFrame->nColorVertexes)
					{
						pFrame->pColorVertexes =new CVector[pFrame->nColorVertexes];
					}
				}
			break;
			case ASE_MESH_CVERTLIST:	{SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_MESH_VERTCOL: 
				{
					unsigned int iColorVertexIndex=ReadDWord();
					pFrame->pColorVertexes[iColorVertexIndex]=ReadVector();
				}
			break;	
			case ASE_MESH_NUMCVFACES:
				{
					pFrame->nColorFaces = ReadDWord();
					if(pFrame->nColorFaces)
					{
						pFrame->pColorFaces =new S3DSColorFace[pFrame->nColorFaces];
					}
				}
			break;
			case ASE_MESH_CFACELIST:	{SKIP_ASE_TOKEN(); /* Skip '{'*/}break;
			case ASE_MESH_CFACE:
				{
					unsigned int dwColorFaceIndex=ReadDWord();
					pFrame->pColorFaces[dwColorFaceIndex].nFaceIndex=dwColorFaceIndex;
					pFrame->pColorFaces[dwColorFaceIndex].pColorVertexes[0]=ReadDWord();
					pFrame->pColorFaces[dwColorFaceIndex].pColorVertexes[1]=ReadDWord();
					pFrame->pColorFaces[dwColorFaceIndex].pColorVertexes[2]=ReadDWord();
				}
			break;
			case ASE_MESH_SMOOTHING:
				{
					const char *pSmoothingOptionalParam=strtok(NULL,ASE_FILE_DELIMITER);
					if(pSmoothingOptionalParam)
					{
						pFrame->pbFaceSmooth[dwFaceIndex]=(atoi(pSmoothingOptionalParam)!=0);
						if(strcmp(pSmoothingOptionalParam,"*MESH_MTLID")==0)
						{
							pFrame->pFaceSubMaterials[dwFaceIndex]=ReadDWord();
						}
					}
				}
			break;
			case ASE_MESH_MTLID:
			{
				pFrame->pFaceSubMaterials[dwFaceIndex]=ReadDWord();
			}
			break;
			
			case ASE_WIREFRAME_COLOR:
				{pObject->vWireframeColor=ReadVector();}break;

			// Materiales
			case ASE_MATERIAL_LIST:
				SKIP_ASE_TOKEN(); // Skip '{'
				break;
			case ASE_MATERIAL: 
				{
					dwCurrentMaterialId=ReadDWord();
					pMaterial=new S3DSMaterial;
					pMaterial->dwMaterialId=dwCurrentMaterialId;
					m_vMaterials.push_back(pMaterial);
					SKIP_ASE_TOKEN(); // Skip '{'
				}
			break;
			case ASE_MAP_DIFFUSE: 
				{
					SKIP_ASE_TOKEN(); // Skip '{'
					dwCurrentBitmap=ASE_MAP_DIFFUSE;
				}
			break;
			case ASE_MAP_BUMP: 
			{
				SKIP_ASE_TOKEN(); // Skip '{'
				dwCurrentBitmap=ASE_MAP_BUMP;
			}
			break;
			case ASE_MAP_OPACITY:
 				{
					SKIP_ASE_TOKEN(); // Skip '{'
					dwCurrentBitmap=ASE_MAP_OPACITY;
				}
			break;
			case ASE_BITMAP: 
				{
					char sFileName[MAX_PATH]={0};
					string fileName=ReadString();
					if(dwCurrentBitmap==ASE_MAP_DIFFUSE)
					{
						GetFileName(fileName.c_str(),sFileName);
						strcpy(pMaterial->sFile,sFileName);
					}
					if(dwCurrentBitmap==ASE_MAP_BUMP)
					{
						GetFileName(fileName.c_str(),sFileName);
						strcpy(pMaterial->sNormalFile,sFileName);
					}
					if(dwCurrentBitmap==ASE_MAP_OPACITY)
					{
						GetFileName(fileName.c_str(),sFileName);
						strcpy(pMaterial->sAlphaFile,sFileName);
					}
				}
			break;
			case ASE_SUBMATERIAL: 
				{
					if(!pMaterial->bSubMaterial)
					{
						// Se elimina el padre y se lee este material de cero
						delete pMaterial; 
						m_vMaterials.pop_back();
					}
					pMaterial=new S3DSMaterial;
					pMaterial->bSubMaterial=true;
					pMaterial->dwMaterialId=dwCurrentMaterialId;
					pMaterial->dwSubMaterialId=ReadDWord();;
					m_vMaterials.push_back(pMaterial);
					SKIP_ASE_TOKEN(); // Skip '{'
				}
			break;
			case ASE_MATERIAL_NAME:			{strcpy(pMaterial->sName,ReadString().c_str());}break;
			case ASE_MATERIAL_AMBIENT:		{pMaterial->vAmbientColor=ReadVector();}break;
			case ASE_MATERIAL_DIFFUSE:		{pMaterial->vDiffuseColor=ReadVector();}break;
			case ASE_MATERIAL_SPECULAR:		{pMaterial->vSpecularColor=ReadVector();}break;
			case ASE_MATERIAL_SHINE:		{pMaterial->fShininess=ReadFloat();}break;
			case ASE_MATERIAL_SHINESTRENGTH:{pMaterial->fShininessStrength=ReadFloat();}break;
			case ASE_MATERIAL_TRANSPARENCY:	{pMaterial->fTranparency=ReadFloat();}break;
			case ASE_MATERIAL_TWOSIDED:		{pMaterial->bTwoSided=true;}break;
			case ASE_UVW_U_OFFSET:			{if(dwCurrentBitmap==ASE_MAP_DIFFUSE){pMaterial->fTextureUOffset=ReadFloat();}}break;
			case ASE_UVW_V_OFFSET:			{if(dwCurrentBitmap==ASE_MAP_DIFFUSE){pMaterial->fTextureVOffset=ReadFloat();}}break;
			case ASE_UVW_U_TILING:			{if(dwCurrentBitmap==ASE_MAP_DIFFUSE){pMaterial->fTextureUScale=ReadFloat();}}break;
			case ASE_UVW_V_TILING:			{if(dwCurrentBitmap==ASE_MAP_DIFFUSE){pMaterial->fTextureVScale=ReadFloat();}}break;
			case ASE_UVW_ANGLE:				{if(dwCurrentBitmap==ASE_MAP_DIFFUSE){pMaterial->fTextureAngle=ReadFloat();}}break;

			// Luces
			case ASE_LIGHTOBJECT:
				{
					SKIP_ASE_TOKEN(); // Skip '{'
					SKIP_ASE_TOKEN(); // Skip *NODE_NAME
					pObject=NULL;
					pCamera=NULL;
					pLight=new S3DSLight;
					pTransformMatrix=&pLight->mTransformMatrix;
					pAxises=pLight->vAxises;
					pCurrentOrigin=&pLight->vPosition;
					pRotationAxis=&pLight->vRotationAxis;
					pRotationAngle=&pLight->fRotationAngle;
					pScale=&pLight->vScale;
					m_vLights.push_back(pLight);
					strcpy(pLight->sName,ReadString().c_str());
				}
			break;
			case ASE_LIGHT_TYPE:
				{
					char *pType=strtok(NULL,ASE_FILE_DELIMITER);
					if(strcmp(pType,"Omni")==0)		{pLight->dwType=S3DS_LIGHT_TYPE_OMNI;}
					if(strcmp(pType,"Target")==0)	{pLight->dwType=S3DS_LIGHT_TYPE_SPOT;}
					if(strcmp(pType,"Free")==0)		{pLight->dwType=S3DS_LIGHT_TYPE_FREE;}
				}
			break;
			case ASE_LIGHT_USELIGHT:
				{
					pLight->bIsOff=(ReadDWord()==0);
				}
			break;
			case ASE_LIGHT_SETTINGS:
				SKIP_ASE_TOKEN(); // Skip '{'
			break;
			case ASE_LIGHT_COLOR:
				pLight->vColor=ReadVector();
			break;
			case ASE_LIGHT_INTENS:
				pLight->fMultiplier=ReadFloat();
			break;
			case ASE_LIGHT_HOTSPOT:
				pLight->fSpotHotSpot=ReadFloat();
			break;
			case ASE_LIGHT_FALLOFF:
				pLight->fSpotFallOff=ReadFloat();
			break;
			case ASE_LIGHT_ATTNSTART:
				pLight->bAttenuationOn=true;
				pLight->fAttenuationStart=ReadFloat();
			break;
			case ASE_LIGHT_ATTNEND:
				pLight->bAttenuationOn=true;
				pLight->fAttenuationEnd=ReadFloat();
			break;
			case ASE_LIGHT_TDIST:
				pLight->fTargetDist=ReadFloat();
			break;
		}
	}
	for(x=0;x<m_vObjects.size();x++)
	{
		pObject=m_vObjects[x];

		for (y=0;y<pObject->baseFrame.nVertexes;y++){From3DSToOpenGL(&pObject->baseFrame.pVertexes[y]);}				
		if(pObject->baseFrame.pVertexNormals){for (y=0;y<pObject->baseFrame.nFaces*3;y++){From3DSToOpenGL(&pObject->baseFrame.pVertexNormals[y]);}}
		if(pObject->baseFrame.pFaceNormals){for (y=0;y<pObject->baseFrame.nFaces;y++){From3DSToOpenGL(&pObject->baseFrame.pFaceNormals[y]);}}

		// Conversion sistema de coordenadas
		for(unsigned int f=0;f<pObject->vFrameInstances.size();f++)
		{
			S3DSFrame *pFrame=pObject->vFrameInstances[f];
			for (y=0;y<pFrame->nVertexes;y++){From3DSToOpenGL(&pFrame->pVertexes[y]);}				
			if(pFrame->pVertexNormals){for (y=0;y<pFrame->nVertexes;y++){From3DSToOpenGL(&pFrame->pVertexNormals[y]);}}
			if(pFrame->pFaceNormals){for (y=0;y<pFrame->nFaces;y++){From3DSToOpenGL(&pFrame->pFaceNormals[y]);}}
		}
	}
	m_Scene.nFrameCount=(int)(bBuildAnimation?m_Scene.dLastFrame-m_Scene.dFirstFrame:1);
	// Construccion del buffer de frames, si un objeto no aparece en un frame se pone un puntero a un frame anterior
	for(x=0;x<m_vObjects.size();x++)
	{
		S3DSObject *pObject=m_vObjects[x];
		S3DSFrame *pFrame=&pObject->baseFrame;
		pObject->vAnimationFrames.resize(m_Scene.nFrameCount);

		for(int f=0;f<m_Scene.nFrameCount;f++)
		{
			for(unsigned int of=0;of<pObject->vFrameInstances.size();of++)
			{
				S3DSFrame *pTempFrame=pObject->vFrameInstances[of];
				if(pTempFrame->dTimeStamp==((double)f*m_Scene.dTicksPerFrame))
				{
					pFrame=pTempFrame;
				}
			}
			pObject->vAnimationFrames[f]=pFrame;
		}
	}

	for(x=0;x<m_vLights.size();x++)
	{
		pLight=m_vLights[x];
		From3DSToOpenGL(&pLight->vPosition);
		From3DSToOpenGL(&pLight->vAxises[0]);
		From3DSToOpenGL(&pLight->vAxises[1]);
		From3DSToOpenGL(&pLight->vAxises[2]);
		From3DSToOpenGL(&pLight->vSpotTarget);
		// si no tiene objetivo se calcula a partir de la matriz y la distancia
		if(!pLight->bHasTarget)
		{
			CMatrix m;
			CVector dir;
			dir=CVector(0,0,-1);
			m.R(pLight->vRotationAxis,-pLight->fRotationAngle,Origin);
			dir*=m;
			m.S(pLight->vScale.c[0],pLight->vScale.c[1],pLight->vScale.c[2],Origin);
			dir*=m;
			From3DSToOpenGL(&dir);
			pLight->vSpotTarget=pLight->vPosition+dir*(double)pLight->fTargetDist;
		}
	}

	for(x=0;x<m_vObjects.size();x++)
	{
		pObject=m_vObjects[x];
		ProcessObjectFrameSubMaterials(pObject,&pObject->baseFrame);
		for(unsigned int f=0;f<pObject->vFrameInstances.size();f++)
		{
			S3DSFrame *pFrame=pObject->vFrameInstances[f];
			ProcessObjectFrameSubMaterials(pObject,pFrame);
		}
	}
	
	if(m_pBuffer){delete [] m_pBuffer;m_pBuffer=NULL;}

	fclose (m_pFile); // Closes the file stream
	m_pFile=NULL;
	m_pBuffer=NULL;
	return true;
}

void CASEFileType::ProcessObjectFrameSubMaterials(S3DSObject *pObject,S3DSFrame *pFrame)
{
	// Mapeo de los materiales de los poligonos de cada objetos en una lista de ObjectMaterials
	// que contienen las caras que tienen cada material en el objeto
	for(unsigned int y=0;y<m_vMaterials.size();y++)
	{
		// Se buscan los materiales cuyo indice de material coincide con el del objeto.
		// si tiene varios submateriales se encontrara mas de uno
		S3DSMaterial *pMaterial=m_vMaterials[y];
		if(pMaterial->dwMaterialId==pObject->dwMaterialId)
		{
			// Se miran cuantas caras tienen el submaterial, y si lo tienen se añade a la lista de materiales
			// del objeto y se construye el buffer de caras con el material.
			int nFacesWithSubMaterial=0;
			if(pMaterial->bSubMaterial)
			{
				for(int f=0;f<pFrame->nFaces;f++)
				{
					if(pFrame->pFaceSubMaterials[f]==pMaterial->dwSubMaterialId)
					{
						nFacesWithSubMaterial++;
					}
				}
			}
			else
			{
				nFacesWithSubMaterial=pFrame->nFaces;
			}
			if(!pMaterial->bSubMaterial ||nFacesWithSubMaterial)
			{
				S3DSObjectMaterial *pFrameMaterial=new S3DSObjectMaterial;
				strcpy(pFrameMaterial->sName,pMaterial->sName);
				pFrameMaterial->dwSubMaterialId=pMaterial->dwSubMaterialId;
				pFrameMaterial->nFaces=nFacesWithSubMaterial;
				pFrameMaterial->pFaces=new int [nFacesWithSubMaterial];

				int nFacesAdded=0;
				for(int f=0;f<pFrame->nFaces;f++)
				{
					if(!pMaterial->bSubMaterial || pFrame->pFaceSubMaterials[f]==pMaterial->dwSubMaterialId)
					{
						pFrameMaterial->pFaces[nFacesAdded]=f;
						nFacesAdded++;
					}
				}
				pFrame->sObjectMaterials.push_back(pFrameMaterial);
			}
		}
	}
}

class CASEToGCMKey
{
public:
	double c[3];double t[2];double n[3];double col[3];unsigned int i;
	bool operator<(const CASEToGCMKey &v2) const{if(c[0]>v2.c[0]){return false;}if(c[0]<v2.c[0]){return true;}if(c[1]>v2.c[1]){return false;}if(c[1]<v2.c[1]){return true;}if(c[2]>v2.c[2]){return false;}if(c[2]<v2.c[2]){return true;}if(t[0]>v2.t[0]){return false;}if(t[0]<v2.t[0]){return true;}if(t[1]>v2.t[1]){return false;}if(t[1]<v2.t[1]){return true;}if(n[0]>v2.n[0]){return false;}if(n[0]<v2.n[0]){return true;}if(n[1]>v2.n[1]){return false;}if(n[1]<v2.n[1]){return true;}if(n[2]>v2.n[2]){return false;}if(n[2]<v2.n[2]){return true;}return false;}
	CASEToGCMKey(CVector vc,CVector vt,CVector vn,CVector vcol,int vi){int x;for(x=0;x<3;x++){c[x]=vc.c[x];}for(x=0;x<2;x++){t[x]=vt.c[x];}for(x=0;x<3;x++){n[x]=vn.c[x];}for(x=0;x<3;x++){col[x]=vcol.c[x];}i=vi;}
	CASEToGCMKey(){memset(t,0,sizeof(t));i=0;}
};
/*
void CalculateTangentArray(long vertexCount, const Point3D *vertex, const Vector3D *normal,
	const Point2D *texcoord, long triangleCount, const Triangle *triangle, Vector4D *tangent)
{
	Vector3D *tan1 = new Vector3D[vertexCount * 2];
	Vector3D *tan2 = tan1 + vertexCount;
	ZeroMemory(tan1, vertexCount * sizeof(Vector3D) * 2);

	for (long a = 0; a < triangleCount; a++)
	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];

		const Point3D& v1 = vertex[i1];
		const Point3D& v2 = vertex[i2];
		const Point3D& v3 = vertex[i3];

		const Point2D& w1 = texcoord[i1];
		const Point2D& w2 = texcoord[i2];
		const Point2D& w3 = texcoord[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		Vector3D sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		Vector3D tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;

		triangle++;
	}

	for (long a = 0; a < vertexCount; a++)
	{
		const Vector3D& n = normal[a];
		const Vector3D& t = tan1[a];

		// Gram-Schmidt orthogonalize
		tangent[a] = (t - n * Dot(n, t)).Normalize();

		// Calculate handedness
		tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}

	delete[] tan1;
}*/

void CASEFileType::ToGCM(CGCMFileType *pFile)
{
	pFile->Clear();
	
	
	for (int f=0;f<m_Scene.nFrameCount;f++)
	{
		unsigned long nFrame=pFile->AddFrame();
		
		for (int m=-1;m<(int)m_vMaterials.size();m++)
		{
			CVector vAmbientColor=CVector(0.5,0.5,0.5);
			CVector vDiffuseColor=CVector(0.5,0.5,0.5);
			CVector vSpecularColor=CVector(0.5,0.5,0.5);
			float fShininess=1;
			float fOpacity=1;
			
			// Preparacion del material
			S3DSMaterial *p3DSMaterial=(m>=0)?m_vMaterials[m]:NULL;
			
			CMatrix ms,mr;
			if(p3DSMaterial)
			{
				// From http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=172040
				//			Shininess = exp (MATERIAL_SHINE * 100.0 / 8.0 * log (2.0));
				//			EffectiveSpecularColor = MATERIAL_SHINESTRENGTH * MATERIAL_SPECULAR + (1 - MATERIAL_SHINESTRENGTH) * MATERIAL_DIFFUSE;
				
				vAmbientColor=p3DSMaterial->vAmbientColor;
				vDiffuseColor=p3DSMaterial->vDiffuseColor;
				vSpecularColor=p3DSMaterial->vSpecularColor;
				fShininess=p3DSMaterial->fShininess*64.0;
				fOpacity=(float)(1.0-p3DSMaterial->fTranparency);
				
				// Calculo de las matrices de transformacion de las coordenadas de textura
				CVector vTextureCenter(0.5,0.5,0);
				ms.S(p3DSMaterial->fTextureUScale,p3DSMaterial->fTextureVScale,1,vTextureCenter);
				mr.R(CVector(0,0,1),p3DSMaterial->fTextureAngle,vTextureCenter);
			}
			
			// Contabilizacion de las caras de todos los objetos para este material 
			int nMaterialFaces=0;
			for (unsigned int o=0;o<m_vObjects.size();o++)
			{
				S3DSObject *pObject=m_vObjects[o];
				if (pObject->bVisible && p3DSMaterial && pObject->dwMaterialId==p3DSMaterial->dwMaterialId)
				{
					if(p3DSMaterial->bSubMaterial)
					{
						S3DSFrame  *p3DSFrame=m_vObjects[o]->vAnimationFrames[f];
						for(unsigned int s=0;s<p3DSFrame->sObjectMaterials.size();s++)
						{
							if(p3DSFrame->sObjectMaterials[s]->dwSubMaterialId==p3DSMaterial->dwSubMaterialId)
							{
								nMaterialFaces+=p3DSFrame->sObjectMaterials[s]->nFaces;
								break;
							}
						}
					}
					else
					{
						nMaterialFaces+=pObject->vAnimationFrames[f]->nFaces;
					}
				}
				else if(!p3DSMaterial)
				{
					nMaterialFaces+=pObject->vAnimationFrames[f]->nFaces;
				}
			}
			
			// Prealocacion con el maximo posible de vertices (caras*3)
			// Despues se realocaran estos buffers a su tamaño correcto
			
			unsigned int *pFaceVertexIndexes=new unsigned int [nMaterialFaces*3];
			float *pVertexArray=new float[nMaterialFaces*3*3];
			float *pNormalArray=new float[nMaterialFaces*3*3];
			float *pColorArray=new float[nMaterialFaces*3*4];
			
			unsigned int *pFaceVertexCursor=pFaceVertexIndexes;
			float *pVertexCursor=pVertexArray;
			float *pNormalCursor=pNormalArray;
			float *pColorCursor=pColorArray;
			float *pTextCursor=NULL;
			float *pTexVertexArray=NULL;
			
			if (p3DSMaterial && p3DSMaterial->sFile[0]!=0)
			{
				pTexVertexArray=new float[nMaterialFaces*3*2];
				pTextCursor=pTexVertexArray;
			}
			
			// Alimentacion de los buffers
			//
			// Aqui se utiliza un mapa de vertices, ya que en los .ASE
			// se da la geometria por un lado y la caracteristicas esteticas 
			// por otro (colores y coordenadas de texturas)
			// Hay caras de color y coords de textura con colores y coordenadas 
			// para los vertices en ESA CARA en concreto
			// Esto se almacena asi en los ase para poder dar coordenadas de textura
			// y colores independientes a caras que comparten vertices.
			// 
			// Como en opengl los vertex buffers solo pueden especificar un color y 
			// coordenada de textura por vertice hay que duplicar los vertices que tengan 
			// propiedades diferentes en mas de una cara.
			//
			// El numero de caras no varia, pero si puede hacerlo el numero de vertices
			//
			// Mediante el mapa se identifican los vertices con varias propiedades,
			// ya que la clave incluye las coordenadas del vertice, su color y sus coords de textura
			
			
			std::map<CASEToGCMKey,unsigned int> mVertexes;
			std::map<CASEToGCMKey,unsigned int>::iterator i;
			
			for (unsigned int o=0;o<m_vObjects.size();o++)
			{
				S3DSObject *pObject=m_vObjects[o];
				if (!pObject->bVisible || p3DSMaterial==NULL || pObject->dwMaterialId!=p3DSMaterial->dwMaterialId) {continue;}
				
				//				S3DSMaterial		*p3DSMaterial=(pObject->dwMaterialId!=(unsigned int)-1)?m_vMaterials[pObject->dwMaterialId]:NULL;
				S3DSFrame  		   	*p3DSFrame=m_vObjects[o]->vAnimationFrames[f];
				
				// Se crean arrays auxiliares de punteros a las propiedades de color y coord de textura
				// de cada cara, para poder acceder a ellas por su indice de cara.
				
				S3DSColorFace 		**ppColorFaces=new S3DSColorFace *[p3DSFrame->nFaces];
				S3DSTextureFace		**ppTextureFaces=new S3DSTextureFace *[p3DSFrame->nFaces];
				
				memset(ppColorFaces,0,sizeof(S3DSColorFace *)*p3DSFrame->nFaces);
				memset(ppTextureFaces,0,sizeof(S3DSTextureFace *)*p3DSFrame->nFaces);
				
				// Creacion del buffer auxiliar de punteros a las propiedades de color de cada cara
				for (int z=0;z<p3DSFrame->nColorFaces;z++)
				{
					if (p3DSFrame->pColorFaces[z].nFaceIndex>=0 && p3DSFrame->pColorFaces[z].nFaceIndex<p3DSFrame->nFaces)
					{
						ppColorFaces[p3DSFrame->pColorFaces[z].nFaceIndex]=&p3DSFrame->pColorFaces[z];
					}
				}
				// Creacion del buffer auxiliar de punteros a las coordenadas de textura de cada cara
				for (int z=0;z<p3DSFrame->nTextFaces;z++)
				{
					if (p3DSFrame->pTextFaces[z].nFaceIndex>=0 && p3DSFrame->pTextFaces[z].nFaceIndex<p3DSFrame->nFaces)
					{
						ppTextureFaces[p3DSFrame->pTextFaces[z].nFaceIndex]=&p3DSFrame->pTextFaces[z];
					}
				}
				
				float ambient[]={(float)vDiffuseColor.c[0],(float)vDiffuseColor.c[1],(float)vDiffuseColor.c[2]};
				float opacity=fOpacity;	
				
				// Procesado de la caras,se van alimentando simultaneamente el mapa
				// de vertices y los buffers.
				
				S3DSObjectMaterial *pObjectMaterial=NULL;
				for(unsigned int s=0;s<p3DSFrame->sObjectMaterials.size();s++)
				{
					if(p3DSFrame->sObjectMaterials[s]->dwSubMaterialId==p3DSMaterial->dwSubMaterialId)
					{
						pObjectMaterial=p3DSFrame->sObjectMaterials[s];
						break;
					}
				}
				
				for (int x=0;pObjectMaterial && x<pObjectMaterial->nFaces;x++)
				{
					int nFaceIndex=pObjectMaterial->pFaces[x];
					S3DSColorFace *pColorFace=ppColorFaces[nFaceIndex];
					S3DSTextureFace *pTextFace=ppTextureFaces[nFaceIndex];
					
					CVector vFaceFlatNormal;
					//Si no hay normales para los vertices o el tipo de shadding es Flat/Solid
					if(!p3DSFrame->pVertexNormals || !p3DSFrame->pbFaceSmooth[nFaceIndex])
					{
						if(p3DSFrame->pFaceNormals)
						{
							vFaceFlatNormal=p3DSFrame->pFaceNormals[nFaceIndex];
						}
						else
						{
							// Si no existe informacion de normales se calcula la normal de la cara
							int nVertexes[3];
							nVertexes[0]=p3DSFrame->pFaces[(nFaceIndex*3)];
							nVertexes[1]=p3DSFrame->pFaces[(nFaceIndex*3)+1];
							nVertexes[2]=p3DSFrame->pFaces[(nFaceIndex*3)+2];
							CPlane plane(p3DSFrame->pVertexes[nVertexes[2]],p3DSFrame->pVertexes[nVertexes[1]],p3DSFrame->pVertexes[nVertexes[0]]);
							vFaceFlatNormal=plane;
						}
					}
					
					for (int v=0;v<3;v++)
					{
						int nSourceVertexIndex=p3DSFrame->pFaces[(nFaceIndex*3)+v];
						
						CASEToGCMKey key;
						key.c[0]=p3DSFrame->pVertexes[nSourceVertexIndex].c[0];
						key.c[1]=p3DSFrame->pVertexes[nSourceVertexIndex].c[1];
						key.c[2]=p3DSFrame->pVertexes[nSourceVertexIndex].c[2];
						
						if(p3DSFrame->pVertexNormals && p3DSFrame->pbFaceSmooth[x])
						{
							key.n[0]=p3DSFrame->pVertexNormals[(nFaceIndex*3)+v].c[0];
							key.n[1]=p3DSFrame->pVertexNormals[(nFaceIndex*3)+v].c[1];
							key.n[2]=p3DSFrame->pVertexNormals[(nFaceIndex*3)+v].c[2];
						}
						else
						{
							key.n[0]=vFaceFlatNormal.c[0];
							key.n[1]=vFaceFlatNormal.c[1];
							key.n[2]=vFaceFlatNormal.c[2];
						}
						
						key.col[0]=pColorFace?p3DSFrame->pColorVertexes[pColorFace->pColorVertexes[v]].c[0]:ambient[0];
						key.col[1]=pColorFace?p3DSFrame->pColorVertexes[pColorFace->pColorVertexes[v]].c[1]:ambient[1];
						key.col[2]=pColorFace?p3DSFrame->pColorVertexes[pColorFace->pColorVertexes[v]].c[2]:ambient[2];
						
						// Aplicacion de la matriz de transformacion de las coordenadas de textura
						CVector vTexTemp;
						if (pTextFace)
						{
							vTexTemp=p3DSFrame->pTextVertexes[pTextFace->pTextVertexes[v]];
							vTexTemp.c[0]-=p3DSMaterial->fTextureUOffset;
							vTexTemp.c[1]-=p3DSMaterial->fTextureVOffset;
							vTexTemp*=ms;
							vTexTemp*=mr;
						}
						key.t[0]=vTexTemp.c[0];
						key.t[1]=vTexTemp.c[1];
						
						int nIndex=0;
						i=mVertexes.find(key);
						if (i==mVertexes.end())
						{
							nIndex=mVertexes.size();
							mVertexes[key]=nIndex;
							(*pVertexCursor++)=(float)key.c[0];
							(*pVertexCursor++)=(float)key.c[1];
							(*pVertexCursor++)=(float)key.c[2];
							(*pNormalCursor++)=(float)key.n[0];
							(*pNormalCursor++)=(float)key.n[1];
							(*pNormalCursor++)=(float)key.n[2];
							(*pColorCursor++)=(float)key.col[0];
							(*pColorCursor++)=(float)key.col[1];
							(*pColorCursor++)=(float)key.col[2];
							(*pColorCursor++)=opacity;
							if(pTextCursor)
							{
								(*pTextCursor++)=(float)key.t[0];
								(*pTextCursor++)=(float)key.t[1];
							}
						}
						else
						{
							nIndex=i->second;
						}
						(*pFaceVertexCursor++)=nIndex;
					}
				}
				delete [] ppColorFaces;
				delete [] ppTextureFaces;
			}
			// Se reducen los buffers de los vertices a su tamaño correcto
			// han sido alocados a su tamaño maximo por comodidad
			
			unsigned int nVertexes=mVertexes.size();
			unsigned int nFaces=nMaterialFaces;
			
			float *pFinalVertexArray=new float[nVertexes*3];
			float *pFinalNormalArray=new float[nVertexes*3];
			float *pFinalColorArray=new float[nVertexes*4];
			
			memcpy(pFinalVertexArray,pVertexArray,sizeof(float)*nVertexes*3);
			memcpy(pFinalNormalArray,pNormalArray,sizeof(float)*nVertexes*3);
			memcpy(pFinalColorArray,pColorArray,sizeof(float)*nVertexes*4);
			
			delete [] pVertexArray;
			delete [] pNormalArray;
			delete [] pColorArray;
			
			pVertexArray=pFinalVertexArray;
			pNormalArray=pFinalNormalArray;
			pColorArray=pFinalColorArray;
			
			// Las coordenadas de textura son opcionales.
			if(pTexVertexArray)
			{
				float *pFinalTextArray=new float[nVertexes*2];
				memcpy(pFinalTextArray,pTexVertexArray,sizeof(float)*nVertexes*2);
				delete [] pTexVertexArray;
				pTexVertexArray=pFinalTextArray;
			}
			
			// Si hay vertices para el material se añade el render buffer 
			// y se carga la textura si hay alguna asignada
			if (nVertexes)
			{
				unsigned long nBuffer=pFile->AddBuffer(nFrame);
				pFile->SetBufferMaterial(nFrame,nBuffer,vAmbientColor,vDiffuseColor,vSpecularColor,fShininess,fOpacity);
				pFile->SetBufferVertexes(nFrame,nBuffer,nVertexes,pVertexArray);
				pFile->SetBufferFaces(nFrame,nBuffer,nFaces,pFaceVertexIndexes);
				pFile->SetBufferColors(nFrame,nBuffer,pColorArray);
				pFile->SetBufferNormals(nFrame,nBuffer,pNormalArray);
				
				if (pTexVertexArray)
				{
					pFile->SetBufferTexture(nFrame,nBuffer,0,p3DSMaterial->sFile);
					pFile->SetBufferTextureCoords(nFrame,nBuffer,0,pTexVertexArray);
				
					if(p3DSMaterial->sNormalFile[0]!=0)
					{
						float *pNormalMapTexArray=new float[nVertexes*2];
						float *pTangentArray=new float[nVertexes*3];
						float *pBitangentArray=new float[nVertexes*3];
						memcpy(pNormalMapTexArray,pTexVertexArray,sizeof(float)*nVertexes*2);
						
						ComputeTangentBasis(nVertexes,pVertexArray,nFaces,pFaceVertexIndexes,pNormalMapTexArray,pNormalArray,pTangentArray,pBitangentArray);

						pFile->SetBufferNormalMap(nFrame,nBuffer,p3DSMaterial->sNormalFile);
						pFile->SetBufferNormalMapCoords(nFrame,nBuffer,pNormalMapTexArray);

						// Calculo de la tangente y bitangente para el normal mapping
						pFile->SetBufferTangents(nFrame,nBuffer,pTangentArray);
						pFile->SetBufferBitangents(nFrame,nBuffer,pBitangentArray);
					}
				}
			}
			else
			{
				delete [] pVertexArray;
				delete [] pFaceVertexIndexes;
				delete [] pColorArray;
				delete [] pNormalArray;
				delete [] pTexVertexArray;
			}
		}
	}	
}
