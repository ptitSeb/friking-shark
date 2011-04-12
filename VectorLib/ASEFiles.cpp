// ASEFileType.cpp: implementation of the CASEFileType class.
//
//////////////////////////////////////////////////////////////////////


#include "./StdAfx.h"
#include "VectorLib.h"
#include "3DSTypes.h"
#include "3DSFiles.h"
#include "ASEFiles.h"
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
	unsigned int				dwCurrentMaterialId=0;
	unsigned int				dwFaceIndex=0;
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
					memset(pFrame->pbFaceSmooth,0,sizeof(bool)*pFrame->nFaces);
					pFrame->pEdges=new bool [pFrame->nFaces*3];
					pFrame->pFaceSubMaterials=new unsigned int[pFrame->nFaces];
					pFrame->pEdges[0]=1;
					pFrame->pEdges[1]=1;
					pFrame->pEdges[2]=1;
				}
			break;

			case ASE_MESH_NORMALS:	
				{
					SKIP_ASE_TOKEN(); /* Skip '{'*/
					pFrame->pFaceNormals=new CVector[pFrame->nFaces];
					pFrame->pVertexNormals = new CVector[pFrame->nVertexes];
				}
			break;
			case ASE_MESH_VERTEXNORMAL:
				{
					unsigned int dwVertexIndex=ReadDWord();
					pFrame->pVertexNormals[dwVertexIndex]=ReadVector();
				}
			break;
			case ASE_MESH_FACENORMAL:
				{
					unsigned int dwTempFaceIndex=ReadDWord();
					pFrame->pFaceNormals[dwTempFaceIndex]=ReadVector();
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
					const char *pSmoothingOptionalParam=ReadString().c_str();
					if(pSmoothingOptionalParam)
					{
						pFrame->pbFaceSmooth[dwFaceIndex]=atoi(pSmoothingOptionalParam);
						if(strcmp(pSmoothingOptionalParam,"*MESH_MTLID")==0)
						{
							pFrame->pFaceSubMaterials[dwFaceIndex]=ReadDWord();
						}
					}
				}
			break;
			case ASE_MESH_MTLID:{pFrame->pFaceSubMaterials[dwFaceIndex]=ReadDWord();}break;
			
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
		if(pObject->baseFrame.pVertexNormals){for (y=0;y<pObject->baseFrame.nVertexes;y++){From3DSToOpenGL(&pObject->baseFrame.pVertexNormals[y]);}}
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
			// Se miran cuantas caras tienen el submaterial, y si lo tienen se a�ade a la lista de materiales
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