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


// 3DSFiles.cpp: implementation of the C3DSFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include "VectorLib.h"
#include "3DSTypes.h"
#include "3DSFiles.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

#define MAIN3DS       0x4D4D

//>------ Main Chunks

#define EDIT3DS       0x3D3D  // this is the start of the editor config
#define KEYF3DS       0xB000  // this is the start of the keyframer config

#define COLOR3DS_FLOAT					0x0010
#define COLOR3DS_BYTE					0x0011
#define COLOR3DS_BYTE_GAMMA_CORRECTED	0x0012
#define COLOR3DS_FLOAT_GAMMA_CORRECTED	0x0013

#define PERCENTAGE_INT					0x0030
#define PERCENTAGE_FLOAT				0x0031


//>------ sub defines of EDIT3DS

#define EDIT_MATERIAL 0xAFFF
#define EDIT_CONFIG1  0x0100
#define EDIT_CONFIG2  0x3E3D
#define EDIT_VIEW_P1  0x7012
#define EDIT_VIEW_P2  0x7011
#define EDIT_VIEW_P3  0x7020
#define EDIT_VIEW1    0x7001
#define EDIT_BACKGR   0x1200
#define EDIT_AMBIENT  0x2100
#define EDIT_OBJECT   0x4000

//>------ sub defines of EDIT_MATERIAL
#define MATERIAL_NAME			0xA000 
#define MATERIAL_TEXTURE_MAP	0xA200
#define MATERIAL_FILE_NAME		0xA300
#define MATERIAL_AMBIENT_COLOR	0xA010
#define MATERIAL_DIFUSSE_COLOR	0xA020
#define MATERIAL_SPECULAR_COLOR 0xA030
#define MATERIAL_SHININESS		0xA040
#define MATERIAL_SHININESS_STRENGTH		0xA041

//>------ sub defines of EDIT_OBJECT

#define OBJECT_MODEL   0x4100
#define OBJECT_LIGHT     0x4600
#define OBJECT_CAMERA    0x4700

#define OBJECT_UNKNWN01  0x4010
#define OBJECT_UNKNWN02  0x4012 //>>---- Could be shadow

//>------ sub defines of OBJECT_CAMERA
#define CAM_UNKNWN01  0x4710 // new chunk type
#define CAM_UNKNWN02  0x4720 // new chunk type

//>------ sub defines of OBJECT_LIGHT
#define OBJECT_LIGHT_OFF				0x4620
#define OBJECT_LIGHT_SPOT				0x4610
#define OBJECT_LIGHT_MULTIPLIER			0x465B
#define OBJECT_LIGHT_ATTENUATION_START	0x4659
#define OBJECT_LIGHT_ATTENUATION_END	0x465A
#define OBJECT_LIGHT_ATTENUATION_ON		0x4625

//>------ sub defines of OBJECT_MODEL
#define OBJECT_VERTEX_LIST   0x4110
#define OBJECT_FACE_LIST2    0x4111 // unknown yet
#define OBJECT_FACE_LIST    0x4120
#define OBJECT_MATERIAL_FACES   0x4130 
#define OBJECT_TEXT_VERTEX_LIST   0x4140
#define TRI_SMOOTH    0x4150
#define OBJECT_LOCAL_MATRIX     0x4160
#define TRI_VISIBLE   0x4165

//>>------ sub defs of KEYF3DS

#define KEYF_UNKNWN01 0xB009
#define KEYF_UNKNWN02 0xB00A
#define KEYF_FRAMES						0xB008
#define KEYF_BLOCK_AMBIENT_LIGHT		0xB001
#define KEYF_BLOCK_OBJECT				0xB002
#define KEYF_BLOCK_CAMERA				0xB003
#define KEYF_BLOCK_CAMERA_TARGET		0xB004
#define KEYF_BLOCK_OMNI_LIGHT			0xB005
#define KEYF_BLOCK_SPOT_LIGHT_TARGET	0xB006
#define KEYF_BLOCK_SPOT_LIGHT			0xB007

#define KEYF_OBJECT_INFO	  0xB010
#define KEYF_OBJECT_DUMMYNAME 0xB011
#define KEYF_OBJECT_PIVOT_POINT  0xB013
#define KEYF_OBJECT_UNKNWN02  0xB014
#define KEYF_OBJECT_UNKNWN03  0xB015  
#define KEYF_OBJECT_POSITION  0xB020  
#define KEYF_OBJECT_ROTATION  0xB021  
#define KEYF_OBJECT_SCALE	  0xB022  
#define KEYF_OBJECT_FOV		  0xB023
#define KEYF_OBJECT_ROLL	  0xB024
#define KEYF_OBJECT_COLOR	  0xB025
#define KEYF_OBJECT_MORPH	  0xB026
#define KEYF_OBJECT_HOTSPOT	  0xB027
#define KEYF_OBJECT_FALLOFF	  0xB028
#define KEYF_OBJECT_HIDE	  0xB029

//>>------  these define the different color chunk types
#define COL_RGB  0x0010
#define COL_TRU  0x0011
#define COL_UNK  0x0013 // unknown

C3DSFileType::C3DSFileType()
{
	m_pFile=NULL;
	m_pBuffer=NULL;
	m_bBinaryFile=false;
}

unsigned short			C3DSFileType::ReadWord()  {unsigned short value=0;if(afread (&value, sizeof(value), 1, m_pFile)!=1){return 0;}return value;}
unsigned int			C3DSFileType::ReadDWord() {unsigned int value=0;if(afread (&value, sizeof(value), 1, m_pFile)!=1){return 0;}return value;}
float			C3DSFileType::ReadFloat() {float value=0;if(afread (&value, sizeof(value), 1, m_pFile)!=1){return 0;}return value;}
unsigned char	C3DSFileType::ReadByte()  {unsigned char value=0;if(afread (&value, sizeof(value), 1, m_pFile)!=1){return 0;}return value;}
CVector			C3DSFileType::ReadVector(){CVector v;v.c[0]=ReadFloat();v.c[1]=ReadFloat();v.c[2]=ReadFloat();return v;}

string C3DSFileType::ReadString()
{
	string temp;
	char sName[512]={0};
	int nNameLength=0;
	do
	{
		sName[nNameLength]=0;
		if(afread (&sName[nNameLength], 1, 1, m_pFile)!=1){break;}
		nNameLength++;
	}
	while(sName[nNameLength-1]!=0 && nNameLength<20);
	temp=sName;
	return temp;
}


bool C3DSFileType::Open(const char *sFileName)
{
	Close();

	if ((m_pFile=afopen (sFileName, "rb"))== NULL) return false; //Open the file
	
	unsigned short	wChunckId=0; //Chunk identifier
	unsigned int	dwChunckLength=0; //Chunk lenght

	S3DSObject			*pObject=NULL;
	S3DSObject			*pKeyFrameObject=NULL;
	S3DSFrame			*pFrame=NULL;
	S3DSLight			*pLight=NULL;
	S3DSCamera			*pCamera=NULL;
	S3DSMaterial		*pMaterial=NULL;
	CVector				*pCurrentColor=NULL;
	float				*pCurrentPercent=NULL;
	string				sCurrentObjectName;

	afseek(m_pFile,0,SEEK_END);
    int nLength=aftell(m_pFile);
	afseek(m_pFile,0,SEEK_SET);
	
	while (aftell (m_pFile) < nLength) //Loop to scan the whole file 
	{
		//getche(); //Insert this command for debug (to wait for keypress for each chuck reading)

		if(afread (&wChunckId, 2, 1, m_pFile)!=1){break;} //Read the chunk header
		if(afread (&dwChunckLength, 4, 1, m_pFile)!=1){break;} //Read the lenght of the chunk

		switch (wChunckId)
        {
			case MAIN3DS: 
			case EDIT3DS:
			break;    
			case COLOR3DS_FLOAT:
			case COLOR3DS_FLOAT_GAMMA_CORRECTED:
				{
					*pCurrentColor=ReadVector();
				}
			break;    
			case COLOR3DS_BYTE:
			case COLOR3DS_BYTE_GAMMA_CORRECTED:
				{
					*pCurrentColor=ReadVector();
				}
			break;    
			case PERCENTAGE_INT:
				{
					unsigned short percentage=ReadWord();
					*pCurrentPercent=(float)(((double)percentage)/100.0);
				}
			break;    
			case PERCENTAGE_FLOAT:
				{
					*pCurrentPercent=ReadFloat();
				}
			break;    
			
			case EDIT_OBJECT: 
				{
					sCurrentObjectName=ReadString();
				}
			break;
			case OBJECT_MODEL:
				{
					pObject=new S3DSObject;
					pFrame=&pObject->baseFrame;
					m_vObjects.push_back(pObject);
					strcpy(pObject->sName,sCurrentObjectName.c_str());
				}
			break;
			case OBJECT_LIGHT:
				{
					pLight=new S3DSLight;
					m_vLights.push_back(pLight);
					strcpy(pLight->sName,sCurrentObjectName.c_str());
					pLight->vPosition=ReadVector();
					pCurrentColor=&pLight->vColor;
				}
			break;
			case OBJECT_LIGHT_SPOT:
				{
					pLight->dwType=S3DS_LIGHT_TYPE_SPOT;
					pLight->vSpotTarget=ReadVector();
					pLight->fSpotHotSpot=ReadFloat();
					pLight->fSpotFallOff=ReadFloat();
				}
			break;
			case OBJECT_LIGHT_MULTIPLIER:
				{
					pLight->fMultiplier=ReadFloat();
				}
			break;
			case OBJECT_LIGHT_ATTENUATION_ON:
				{
					pLight->bAttenuationOn=true;
				}
			break;
			case OBJECT_LIGHT_ATTENUATION_START:
				{
					pLight->fAttenuationStart=ReadFloat();
				}
			break;
			case OBJECT_LIGHT_ATTENUATION_END:
				{
					pLight->fAttenuationEnd=ReadFloat();
				}
			break;
			case OBJECT_LIGHT_OFF:
				{
					pLight->bIsOff=true;
				}
			break;
			case OBJECT_CAMERA:
				{
					pCamera=new S3DSCamera;
					m_vCameras.push_back(pCamera);
					strcpy(pCamera->sName,sCurrentObjectName.c_str());
					pCamera->vPosition=ReadVector();
					pCamera->vTarget=ReadVector();
					pCamera->fRoll=ReadFloat();
					pCamera->fLens=ReadFloat();
				}
			break;
			case OBJECT_VERTEX_LIST: 
				{
					pFrame->nVertexes = ReadWord();
					pFrame->pVertexes =new CVector[pFrame->nVertexes];
					for (int i=0; i<pFrame->nVertexes; i++)
					{
						pFrame->pVertexes[i]=ReadVector();
					}
				}
				break;	
			case OBJECT_TEXT_VERTEX_LIST:
				{
					pFrame->nTextVertexes=ReadWord();
					if(pFrame->nTextVertexes)
					{
						pFrame->pTextVertexes=new CVector[pFrame->nTextVertexes];
						for (int i=0; i<pFrame->nTextVertexes; i++)
						{
							pFrame->pTextVertexes[i].c[0]=ReadFloat();
							pFrame->pTextVertexes[i].c[1]=ReadFloat();
						}
					}
				}
                break;

			case OBJECT_FACE_LIST:
				{
					pFrame->nFaces = ReadWord();
					pFrame->pFaces=new int[pFrame->nFaces*3];
 
					for (int i=0; i<pFrame->nFaces; i++)
					{
						pFrame->pFaces[i*3]=ReadWord();
						pFrame->pFaces[i*3+1]=ReadWord();
						pFrame->pFaces[i*3+2]=ReadWord();
						ReadWord();// flags
					}
				}
                break;
			case OBJECT_LOCAL_MATRIX:
				{
					CVector vectors[4];
					for (int j=0;j<4;j++)
					{
						vectors[j]=ReadVector();
					}
					
					CMatrix temp1,temp2;
					temp1.Ref(vectors[0],vectors[1],vectors[2]);
					temp2.T(Origin-vectors[3]);
					pObject->mTransformMatrix*=temp2;
					pObject->mTransformMatrix*=temp1;
					pObject->vAxises[0]=vectors[0];
					pObject->vAxises[1]=vectors[1];
					pObject->vAxises[2]=vectors[2];
					pObject->vOrigin=vectors[3];
				}
                break;
			case MATERIAL_NAME:
				{
					pMaterial=new S3DSMaterial;
					m_vMaterials.push_back(pMaterial);

					string sName=ReadString();
					strcpy(pMaterial->sName,sName.c_str());
				}
				break;
			case MATERIAL_TEXTURE_MAP:
				break;
			case MATERIAL_FILE_NAME:
				{
					string sName=ReadString();
					strcpy(pMaterial->sFile,sName.c_str());
				}
				break;
			case MATERIAL_AMBIENT_COLOR:
				{
					pCurrentColor=&pMaterial->vAmbientColor;
				}
				break;
			case MATERIAL_DIFUSSE_COLOR:
				{
					pCurrentColor=&pMaterial->vDiffuseColor;
				}
				break;
			case MATERIAL_SPECULAR_COLOR:
				{
					pCurrentColor=&pMaterial->vSpecularColor;
				}
				break;
			case MATERIAL_SHININESS:
				{
					pCurrentPercent=&pMaterial->fShininess;
				}
				break;
			case MATERIAL_SHININESS_STRENGTH:
				{
					pCurrentPercent=&pMaterial->fShininessStrength;
				}
				break;
			case EDIT_MATERIAL:
				break;
			case OBJECT_MATERIAL_FACES:
				{
					string sName=ReadString();
					unsigned short   nFaces=ReadWord();

					S3DSObjectMaterial *pObjectMaterial=new S3DSObjectMaterial;
					strcpy(pObjectMaterial->sName,sName.c_str());

					if(nFaces)
					{
						pObjectMaterial->nFaces=nFaces;
						pObjectMaterial->pFaces=new int[nFaces];
						if(afread (pObjectMaterial->pFaces, sizeof (int)*nFaces, 1, m_pFile)!=1){break;}
					}
					pFrame->sObjectMaterials.push_back(pObjectMaterial);
				}
				break;
			case  KEYF3DS:
				{
				}
				break;
			case  KEYF_FRAMES :
				{
					ReadDWord();// Start
					ReadDWord();// End
				}
				break;
			case  KEYF_BLOCK_OBJECT:
				{
				}
				break;
			case  KEYF_OBJECT_INFO:
				{
					string sName=ReadString();
					unsigned short wFlags[2]={0},wFather=0;
					wFlags[0]=ReadWord();
					wFlags[1]=ReadWord();
					wFather=ReadWord();
					pKeyFrameObject=FindObject(sName.c_str());
				}
				break;
			case  KEYF_OBJECT_PIVOT_POINT:
				{
					CVector vPos=ReadVector();
					if(pKeyFrameObject)
					{
						pKeyFrameObject->vPivotPos=vPos;
					}
				}
				break;
			case  KEYF_OBJECT_POSITION:
			case  KEYF_OBJECT_ROTATION:
			case  KEYF_OBJECT_SCALE	:
			case  KEYF_OBJECT_FOV:
			case  KEYF_OBJECT_ROLL:
			case  KEYF_OBJECT_COLOR:
			case  KEYF_OBJECT_MORPH:
			case  KEYF_OBJECT_HOTSPOT:
			case  KEYF_OBJECT_FALLOFF:
			case  KEYF_OBJECT_HIDE:
				{
					ReadWord();//Flags
					ReadDWord();//dwUnknown1
					ReadDWord();//dwUnknown2
					unsigned int dwKeys=ReadDWord();
					for(unsigned int x=0;x<dwKeys;x++)
					{
						unsigned int dwKeyIndex=ReadDWord();
						ReadWord();//wAccelData
						if(wChunckId==KEYF_OBJECT_POSITION)
						{
							CVector vPos=ReadVector();
							if(dwKeyIndex==0 && pKeyFrameObject!=NULL)
							{
								pKeyFrameObject->bHasFirstFrameInfo=true;
								pKeyFrameObject->vFirstFramePos=vPos;
							}
						}
						else if(wChunckId==KEYF_OBJECT_ROTATION)
						{
							float	fAngle=ReadFloat();
							CVector vAxis=ReadVector();
							if(dwKeyIndex==0 && pKeyFrameObject!=NULL)
							{
								pKeyFrameObject->bHasFirstFrameInfo=true;
								pKeyFrameObject->fFirstFrameRotationAngle=fAngle;
								pKeyFrameObject->vFirstFrameRotationAxis=vAxis;
							}
						}
						else if(wChunckId==KEYF_OBJECT_SCALE)
						{
							CVector vScale=ReadVector();
							if(dwKeyIndex==0 && pKeyFrameObject!=NULL)
							{
								pKeyFrameObject->bHasFirstFrameInfo=true;
								pKeyFrameObject->vFirstFrameScale=vScale;
							}
						}
						else if(wChunckId==KEYF_OBJECT_FOV)
						{
							ReadFloat();
						}
						else if(wChunckId==KEYF_OBJECT_ROLL)
						{
							ReadFloat();
						}
						else if(wChunckId==KEYF_OBJECT_COLOR)
						{
						}
						else if(wChunckId==KEYF_OBJECT_MORPH)
						{
							ReadString();
						}
						else if(wChunckId==KEYF_OBJECT_HOTSPOT)
						{
							ReadFloat();
						}
						else if(wChunckId==KEYF_OBJECT_FALLOFF)
						{
							ReadFloat();
						}
						else if(wChunckId==KEYF_OBJECT_HIDE)
						{
							pKeyFrameObject->bVisible=false;
						}
					}
				}
				break;
			default:
				{
					afseek(m_pFile, dwChunckLength-6, SEEK_CUR);
				}
        } 
	}

	unsigned int x;
	int y;

	for(x=0;x<m_vObjects.size();x++)
	{
		pObject=m_vObjects[x];
		if(pObject->bHasFirstFrameInfo)
		{
			// Aqui se pone el objeto en la posicion correcta del primer frame.
			// Los objetos vienen posicionados en su propio sistema de referencia y movidos a su origen dentro del mundo.
			// para aplicar el pivote hay que mover el objeto al sistema de referencia del mundo  (mediante la 
			// matriz de transformacion que se obtiene de su origen y sistema de coordenadas) 
			// Despues se aplicala traslacion de la distancia al pivote para que influya en el escalado y el rotado 
			// Despues se aplica el escalado y el rotado que indique el primer frame. 
			// Por ultimo se mueve el objeto a su posicion en el frame.

			CMatrix m,mTransform;
			// Cambio a sistema de referencia del mundo (deshacer transformaciones de su propio sistema de referencia y de traslacion)
			mTransform=pObject->mTransformMatrix;
			// Se desplaza la distancia a su pivote para que influya en las tranformaciones de escalado y rotacion
			m.T(Origin-pObject->vPivotPos);
			mTransform*=m;
			// Se escala
			m.S(pObject->vFirstFrameScale.c[0],pObject->vFirstFrameScale.c[1],pObject->vFirstFrameScale.c[2],Origin);
			mTransform*=m;
			// Se Rota
			m.R(pObject->vFirstFrameRotationAxis,-pObject->fFirstFrameRotationAngle,Origin);
			mTransform*=m;
			// Se mueve el objeto a su posicion
			m.T(pObject->vFirstFramePos);
			mTransform*=m;

			for (y=0;y<pObject->baseFrame.nVertexes;y++)
			{
				pObject->baseFrame.pVertexes[y]*=mTransform;
				From3DSToOpenGL(&pObject->baseFrame.pVertexes[y]);
			}				
		}
	}
	for(x=0;x<m_vLights.size();x++)
	{
		S3DSLight *pLight=m_vLights[x];
		double temp=0;
		temp=pLight->vPosition.c[2];
		pLight->vPosition.c[2]=-pLight->vPosition.c[1];
		pLight->vPosition.c[1]=temp;
		temp=pLight->vSpotTarget.c[2];
		pLight->vSpotTarget.c[2]=-pLight->vSpotTarget.c[1];
		pLight->vSpotTarget.c[1]=temp;
	}

	afclose (m_pFile); // Closes the file stream
	m_pFile=NULL;
	m_bBinaryFile=false;
	return true; // Returns ok
}




