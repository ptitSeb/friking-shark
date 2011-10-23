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


// OBJFiles.cpp: Game Compiled Model file loader/saver.
// .gcm files are files designed to store models to be loaded fast.
//
//////////////////////////////////////////////////////////////////////


#include "./StdAfx.h"
#include "VectorLib.h"
#include "OBJFiles.h"
#include "GCMFiles.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OBJ_TOKEN_SEPARATORS "\r\n\t "

class COBJToGCMKey
{
public:
	double c[3];double t[2];double n[3];unsigned int i;
	bool operator<(const COBJToGCMKey &v2) const{if(c[0]>v2.c[0]){return false;}if(c[0]<v2.c[0]){return true;}if(c[1]>v2.c[1]){return false;}if(c[1]<v2.c[1]){return true;}if(c[2]>v2.c[2]){return false;}if(c[2]<v2.c[2]){return true;}if(t[0]>v2.t[0]){return false;}if(t[0]<v2.t[0]){return true;}if(t[1]>v2.t[1]){return false;}if(t[1]<v2.t[1]){return true;}if(n[0]>v2.n[0]){return false;}if(n[0]<v2.n[0]){return true;}if(n[1]>v2.n[1]){return false;}if(n[1]<v2.n[1]){return true;}if(n[2]>v2.n[2]){return false;}if(n[2]<v2.n[2]){return true;}return false;}
	COBJToGCMKey(CVector vc,CVector vt,CVector vn,int vi){int x;for(x=0;x<3;x++){c[x]=vc.c[x];}for(x=0;x<2;x++){t[x]=vt.c[x];}for(x=0;x<3;x++){n[x]=vn.c[x];}i=vi;}
	COBJToGCMKey(){memset(t,0,sizeof(t));i=0;}
};

COBJFileType::COBJFileType()
{
	m_pCurrentMaterial=NULL;
}

COBJFileType::~COBJFileType()
{
	Clear();
}

bool COBJFileType::Open(const char *psFileName)
{
	Clear();

	bool bOk=true;
	FILE *pFile=fopen(psFileName,"rb");
	bOk=(pFile!=NULL);
	if(!bOk){return false;}
	
	m_pCurrentMaterial=NULL;

	char sLine[1024*16]={0};
	while(bOk && fgets(sLine,sizeof(sLine)-1,pFile))
	{
		char *pTemp;
		char *pType=strtok_r(sLine,OBJ_TOKEN_SEPARATORS,&pTemp);
		if(!pType){continue;}
		
		if(strcasecmp(pType,"v")==0)
		{
			char *x=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			char *y=x?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			char *z=y?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			if(z)
			{
				m_vVertexes.push_back(atof(x));
				m_vVertexes.push_back(atof(y));
				m_vVertexes.push_back(atof(z));
			}
		}
		else if(strcasecmp(pType,"vt")==0)
		{
			char *x=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			char *y=x?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			if(y)
			{
				m_vTexCoords.push_back(atof(x));
				m_vTexCoords.push_back(atof(y));
			}
		}
		else if(strcasecmp(pType,"vn")==0)
		{
			char *x=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			char *y=x?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			char *z=y?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			if(z)
			{
				CVector n(atof(x),atof(y),atof(z));
				n.N();
				m_vNormals.push_back(n.c[0]);
				m_vNormals.push_back(n.c[1]);
				m_vNormals.push_back(n.c[2]);
			}
		}
		else if(strcasecmp(pType,"f")==0 && m_pCurrentMaterial)
		{
			char *pVertexTexts[4]={0};
			pVertexTexts[0]=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			pVertexTexts[1]=pVertexTexts[0]?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			pVertexTexts[2]=pVertexTexts[1]?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			pVertexTexts[3]=pVertexTexts[2]?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			
			if(pVertexTexts[2] && !pVertexTexts[3]) // Only triangles are supported
			{
				SOBJFace face;
				for(int x=0;x<3;x++)
				{
					char *pSlash=strchr(pVertexTexts[x],'/');
					if(pSlash)
					{
						*pSlash=0;
						face.pTexCoords[x]=atoi(++pSlash)-1;
						pSlash=strchr(pSlash,'/');
						if(face.pTexCoords[x]>=(int)(m_vTexCoords.size()/2))
						{
							bOk=false;
							RTTRACE("COBJFileType::Open -> Failed to load %s: Invalid texture vertex %d",psFileName,face.pTexCoords[x]+1);
						}						
					}
					if(pSlash)
					{
						*pSlash=0;
						face.pNormals[x]=atoi(++pSlash)-1;
						if(face.pNormals[x]>=(int)(m_vNormals.size()/3))
						{
							bOk=false;
							RTTRACE("COBJFileType::Open -> Failed to load %s: Invalid normal vertex %d",psFileName,face.pNormals[x]+1);
						}						
					}
					face.pVertexes[x]=atoi(pVertexTexts[x])-1;
					if(face.pVertexes[x]>=(int)(m_vVertexes.size()/3))
					{
						bOk=false;
						RTTRACE("COBJFileType::Open -> Failed to load %s: Invalid vertex %d",psFileName,face.pVertexes[x]+1);
					}
				}
				face.pMaterial=m_pCurrentMaterial;
				m_pCurrentMaterial->vFaces.push_back(face);
			}
			else
			{
				bOk=false;
				RTTRACE("COBJFileType::Open -> Failed to load %s: Only triangles are supported",psFileName);
			}
		}
		else if(strcasecmp(pType,"mtllib")==0)
		{
			char *pMTLFile=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			if(pMTLFile)
			{
				bOk=ReadMTL(psFileName,pMTLFile);
				if(!bOk)
				{
					RTTRACE("COBJFileType::Open -> Failed to load %s: File %s not found",psFileName,pMTLFile);
				}
			}
		}
		else if(strcasecmp(pType,"usemtl")==0)
		{
			char *pMaterial=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			if(pMaterial)
			{
				if(m_mMaterials.find(pMaterial)!=m_mMaterials.end())
				{
					m_pCurrentMaterial=m_mMaterials.find(pMaterial)->second;
				}
				else
				{
					bOk=false;
					RTTRACE("COBJFileType::Open -> Failed to load %s: Material %s not found",psFileName,pMaterial);
				}
			}
		}
	}
	if(!bOk){Clear();}
	
	if(pFile)
	{
		fclose(pFile);
		pFile=NULL;
	}
	m_pCurrentMaterial=NULL;
	
	return bOk;
}

bool COBJFileType::ReadMTL(const char *psObjFile, const char *psMTLFile)
{
	std::string sFile=AppendPathSeparator(GetFileFolder(psObjFile).c_str())+GetFileName(psMTLFile);
	bool bOk=true;
	FILE *pFile=fopen(sFile.c_str(),"rb");
	bOk=(pFile!=NULL);
	if(!bOk){return false;}
	
	SOBJMaterial *pMaterial=NULL;
	
	char sLine[1024*16]={0};
	while(fgets(sLine,sizeof(sLine)-1,pFile))
	{
		char *pTemp;
		char *pType=strtok_r(sLine,OBJ_TOKEN_SEPARATORS,&pTemp);
		if(!pType){continue;}
		
		if(strcasecmp(pType,"newmtl")==0)
		{
			char *pName=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
			if(pName)
			{
				if(m_mMaterials.find(pName)==m_mMaterials.end())
				{
					pMaterial=new SOBJMaterial;
					m_mMaterials[pName]=pMaterial;
				}
			}
		}
		else if(pMaterial)
		{
			if(strcasecmp(pType,"Ka")==0)
			{
				char *r=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				char *g=r?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				char *b=g?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				if(b){pMaterial->cAmbient=CVector(atof(r),atof(g),atof(b));}
			}
			else if(strcasecmp(pType,"Kd")==0)
			{
				char *r=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				char *g=r?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				char *b=g?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				if(b){pMaterial->cDiffuse=CVector(atof(r),atof(g),atof(b));}
			}
			else if(strcasecmp(pType,"Ks")==0)
			{
				char *r=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				char *g=r?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				char *b=g?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				if(b){pMaterial->cSpecular=CVector(atof(r),atof(g),atof(b));}
			}
			else if(strcasecmp(pType,"Ns")==0)
			{
				char *s=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				if(s){pMaterial->fShininess=(((float)atof(s))*128.0)/1000.0;}
			}
			else if(strcasecmp(pType,"Tr")==0 || strcasecmp(pType,"d")==0)
			{
				char *d=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				if(d){pMaterial->fOpacity=(float)atof(d);}
			}
			else if(strcasecmp(pType,"map_Kd")==0)
			{
				char *psTexture=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				if(psTexture){pMaterial->sTexture=psTexture;}
			}
			else if(strcasecmp(pType,"map_bump")==0 || strcasecmp(pType,"bump")==0 )
			{
				char *psNormalFile=pType?strtok_r(NULL,OBJ_TOKEN_SEPARATORS,&pTemp):NULL;
				if(psNormalFile){pMaterial->sNormalFile=psNormalFile;}
			}
		}
	}
	if(pFile)
	{
		fclose(pFile);
		pFile=NULL;
	}
	return bOk;
}

void COBJFileType::Clear()
{
	std::map<std::string,SOBJMaterial*>::iterator iMaterial;
	for(iMaterial=m_mMaterials.begin();iMaterial!=m_mMaterials.end();iMaterial++)
	{
		SOBJMaterial *pOBJMaterial=iMaterial->second;
		delete pOBJMaterial;
	}
	
	m_vVertexes.clear();
	m_vTexCoords.clear();
	m_vNormals.clear();
	m_mMaterials.clear();
}

void COBJFileType::ToGCM(CGCMFileType *pFile)
{
	unsigned long nFrame=pFile->AddFrame();
	
	std::map<std::string,SOBJMaterial*>::iterator iMaterial;
	for(iMaterial=m_mMaterials.begin();iMaterial!=m_mMaterials.end();iMaterial++)
	{
		SOBJMaterial *pOBJMaterial=iMaterial->second;
		
		// Prealocacion con el maximo posible de vertices (caras*3)
		// Despues se realocaran estos buffers a su tamaño correcto
		
		unsigned int *pFaceVertexIndexes=new unsigned int [pOBJMaterial->vFaces.size()*3];
		float *pVertexArray=new float[pOBJMaterial->vFaces.size()*3*3];
		float *pNormalArray=new float[pOBJMaterial->vFaces.size()*3*3];
		float *pTexVertexArray=new float[pOBJMaterial->vFaces.size()*3*2];
	
		unsigned int *pFaceVertexCursor=pFaceVertexIndexes;
		float *pVertexCursor=pVertexArray;
		float *pNormalCursor=pNormalArray;
		float *pTextCursor=pTexVertexArray;
		
		std::map<COBJToGCMKey,unsigned int> mVertexes;
		std::map<COBJToGCMKey,unsigned int>::iterator i;

		bool bAnyVertexWithTexture=false;
		
		for (int x=0;x<pOBJMaterial->vFaces.size();x++)
		{
			SOBJFace *pFace=&pOBJMaterial->vFaces[x];
			CVector vFaceFlatNormal;
			CVector vVertexes[3];
			
			for (int v=0;v<3;v++)
			{
				int nSourceVertexBase=pFace->pVertexes[v];
				if(nSourceVertexBase!=-1)
				{
					vVertexes[v].c[0]=m_vVertexes[nSourceVertexBase*3];
					vVertexes[v].c[1]=m_vVertexes[nSourceVertexBase*3+1];
					vVertexes[v].c[2]=m_vVertexes[nSourceVertexBase*3+2];
				}
			}
			
			CPlane plane(vVertexes[2],vVertexes[1],vVertexes[0]);
			vFaceFlatNormal=plane;
			
			for (int v=0;v<3;v++)
			{
				int nSourceVertexBase=pFace->pVertexes[v];
				int nSourceTextureBase=pFace->pTexCoords[v];
				int nSourceNormalBase=pFace->pNormals[v];
				
				COBJToGCMKey key;
				if(nSourceVertexBase!=-1)
				{
					key.c[0]=m_vVertexes[nSourceVertexBase*3];
					key.c[1]=m_vVertexes[nSourceVertexBase*3+1];
					key.c[2]=m_vVertexes[nSourceVertexBase*3+2];
				}
				if(nSourceTextureBase!=-1)
				{
					bAnyVertexWithTexture=true;
					key.t[0]=m_vTexCoords[nSourceTextureBase*2];
					key.t[1]=m_vTexCoords[nSourceTextureBase*2+1];
				}
				
				if(nSourceNormalBase!=-1)
				{
					key.n[0]=m_vNormals[nSourceNormalBase*3];
					key.n[1]=m_vNormals[nSourceNormalBase*3+1];
					key.n[2]=m_vNormals[nSourceNormalBase*3+2];
				}
				else
				{
					key.n[0]=vFaceFlatNormal.c[0];
					key.n[1]=vFaceFlatNormal.c[1];
					key.n[2]=vFaceFlatNormal.c[2];
				}
				
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
					(*pTextCursor++)=(float)key.t[0];
					(*pTextCursor++)=(float)key.t[1];
				}
				else
				{
					nIndex=i->second;
				}
				(*pFaceVertexCursor++)=nIndex;
			}
		}
		
		// Se reducen los buffers de los vertices a su tamaño correcto
		// han sido alocados a su tamaño maximo por comodidad
		
		unsigned int nVertexes=mVertexes.size();
		unsigned int nFaces=pOBJMaterial->vFaces.size();
		
		float *pFinalVertexArray=new float[nVertexes*3];
		float *pFinalNormalArray=new float[nVertexes*3];
		
		memcpy(pFinalVertexArray,pVertexArray,sizeof(float)*nVertexes*3);
		memcpy(pFinalNormalArray,pNormalArray,sizeof(float)*nVertexes*3);
		
		delete [] pVertexArray;
		delete [] pNormalArray;
		
		pVertexArray=pFinalVertexArray;
		pNormalArray=pFinalNormalArray;
		
		
		if(bAnyVertexWithTexture && pOBJMaterial->sTexture!="")
		{
			float *pFinalTextArray=new float[nVertexes*2];
			memcpy(pFinalTextArray,pTexVertexArray,sizeof(float)*nVertexes*2);
			delete [] pTexVertexArray;
			pTexVertexArray=pFinalTextArray;
		}
		else
		{
			delete [] pTexVertexArray;
			pTexVertexArray=NULL;
		}
		
		// Si hay vertices para el material se añade el render buffer 
		// y se carga la textura si hay alguna asignada
		if (nVertexes)
		{
			unsigned long nBuffer=pFile->AddBuffer(nFrame);
			pFile->SetBufferMaterial(nFrame,nBuffer,pOBJMaterial->cAmbient,pOBJMaterial->cDiffuse,pOBJMaterial->cSpecular,pOBJMaterial->fShininess,pOBJMaterial->fOpacity);
			pFile->SetBufferVertexes(nFrame,nBuffer,nVertexes,pVertexArray);
			pFile->SetBufferFaces(nFrame,nBuffer,nFaces,pFaceVertexIndexes);
			pFile->SetBufferNormals(nFrame,nBuffer,pNormalArray);
			
			if (pTexVertexArray)
			{
				pFile->SetBufferTexture(nFrame,nBuffer,0,pOBJMaterial->sTexture);
				pFile->SetBufferTextureCoords(nFrame,nBuffer,0,pTexVertexArray);
				
				
				if(pOBJMaterial->sNormalFile[0]!=0)
				{
					float *pNormalMapTexArray=new float[nVertexes*2];
					float *pTangentArray=new float[nVertexes*3];
					float *pBitangentArray=new float[nVertexes*3];
					memcpy(pNormalMapTexArray,pTexVertexArray,sizeof(float)*nVertexes*2);
					
					ComputeTangentBasis(nVertexes,pVertexArray,nFaces,pFaceVertexIndexes,pNormalMapTexArray,pNormalArray,pTangentArray,pBitangentArray);
					
					pFile->SetBufferNormalMap(nFrame,nBuffer,pOBJMaterial->sNormalFile);
					pFile->SetBufferNormalMapCoords(nFrame,nBuffer,pNormalMapTexArray);
					
					// Calculo de la tangente y bitangente para el normal mapping
					pFile->SetBufferTangents(nFrame,nBuffer,pTangentArray);
					pFile->SetBufferBitangents(nFrame,nBuffer,pBitangentArray);
					
					delete [] pTangentArray;
					delete [] pBitangentArray;
				}
			}
		}
		else
		{
			delete [] pVertexArray;
			delete [] pFaceVertexIndexes;
			delete [] pNormalArray;
			delete [] pTexVertexArray;
		}
	}
}
