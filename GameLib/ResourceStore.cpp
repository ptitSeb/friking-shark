// GameRender.cpp: implementation of the GameRender class.
//
//////////////////////////////////////////////////////////////////////

#include "./StdAfx.h"
#include <float.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include "VectorLib.h"
#include "GameRunTimeLib.h"
#include "ResourceStore.h"

CFrame::CFrame()
{
	m_nPolygons=0;
	m_pPolygons=NULL;
}

CFrame::~CFrame()
{
	if(m_pPolygons){delete [] m_pPolygons;m_pPolygons=NULL;}
	m_nPolygons=0;
}

void CFrame::CalcBBox()
{
	m_vMins=Origin;
	m_vMaxs=Origin;

	for(int x=0;x<m_nPolygons;x++)
	{
		CPolygon *pPolygon=&m_pPolygons[x];
		for(unsigned int v=0;v<pPolygon->m_nVertexes;v++)
		{
			if(x==0 && v==0)
			{
				m_vMins=pPolygon->m_pVertexes[v];
				m_vMaxs=pPolygon->m_pVertexes[v];
			}
			else
			{
				for(unsigned int c=0;c<3;c++)
				{
					if(pPolygon->m_pVertexes[v].c[c]<m_vMins.c[c]){m_vMins.c[c]=pPolygon->m_pVertexes[v].c[c];}
					if(pPolygon->m_pVertexes[v].c[c]>m_vMaxs.c[c]){m_vMaxs.c[c]=pPolygon->m_pVertexes[v].c[c];}
				}
			}
		}
	}
}

class CVertexKey
{
public:
	double c[3]; // Coordenadas
	double t[2];
	double n[3];
	double col[3];
	unsigned int i;

	bool operator<(const CVertexKey &v2) const
	{
		if(c[0]>v2.c[0]){return false;}
		if(c[0]<v2.c[0]){return true;}
		if(c[1]>v2.c[1]){return false;}
		if(c[1]<v2.c[1]){return true;}
		if(c[2]>v2.c[2]){return false;}
		if(c[2]<v2.c[2]){return true;}
		if(t[0]>v2.t[0]){return false;}
		if(t[0]<v2.t[0]){return true;}
		if(t[1]>v2.t[1]){return false;}
		if(t[1]<v2.t[1]){return true;}
		if(n[0]>v2.n[0]){return false;}
		if(n[0]<v2.n[0]){return true;}
		if(n[1]>v2.n[1]){return false;}
		if(n[1]<v2.n[1]){return true;}
		if(n[2]>v2.n[2]){return false;}
		if(n[2]<v2.n[2]){return true;}
		return false;
	}

	CVertexKey(CVector vc,CVector vt,CVector vn,CVector vcol,int vi)
	{
		int x;
		for(x=0;x<3;x++){c[x]=vc.c[x];}
		for(x=0;x<2;x++){t[x]=vt.c[x];}
		for(x=0;x<3;x++){n[x]=vn.c[x];}
		for(x=0;x<3;x++){col[x]=vcol.c[x];}
		i=vi;
	}
	CVertexKey(){memset(c,0,sizeof(c));memset(t,0,sizeof(t));memset(n,0,sizeof(n));memset(col,0,sizeof(col));i=0;}
};

void CFrame::GetGeometry( vector<CPolygon *> *pvGeometry )
{
	for(int x=0;x<m_nPolygons;x++)
	{
		CPolygon *pPolygon=new CPolygon;
		*pPolygon=m_pPolygons[x];
		pPolygon->CalcPlane();
		pvGeometry->push_back(pPolygon);
	}
}

CModelAnimation::CModelAnimation()
{
	m_nFrames=0;
	m_pFrames=NULL;
}

CModelAnimation::~CModelAnimation()
{
	if(m_pFrames){delete [] m_pFrames;m_pFrames=NULL;}
	m_nFrames=0;
}


CModel::CModel()
{
	m_nAnimations=0;
	m_pAnimations=NULL;
}

CModel::~CModel()
{
	Reset();
}

void CModel::Reset()
{
	if(m_pAnimations){delete [] m_pAnimations;m_pAnimations=NULL;}
	m_nAnimations=0;
	m_sName="";
}

CModelAnimation *CModel::GetAnimation(int nAnimation)
{
	if(nAnimation>=m_nAnimations){return NULL;}
	return &m_pAnimations[nAnimation];
}

CFrame	*CModel::GetFrame(int nAnimation,int nFrame)
{
	if(nAnimation>=m_nAnimations){return NULL;}
	CModelAnimation *pAnimation=&m_pAnimations[nAnimation];
	if(pAnimation==NULL){return NULL;}
	if(nFrame>=pAnimation->m_nFrames){return NULL;}
	return &pAnimation->m_pFrames[nFrame];
}

CResourceStore::CResourceStore()
{
    m_sName="ResourceStore";
}

CResourceStore::~CResourceStore()
{
	Destroy();
}

CModel *CResourceStore::LoadModel(string sFileName,CModel *pExistingModel)
{
	char pPath[MAX_PATH];
	GetFileFolder(sFileName.c_str(),pPath);

	char sExt[MAX_PATH]={0};
	GetExtension(sFileName.c_str(),sExt);
	if(pPath[0]==0 || strcmp(pPath,".")==0)
	{
		string sTemp="Models/";
		sTemp+=sFileName;
		sFileName=sTemp;
	}

	CModel *pModel=pExistingModel;
	if(!pExistingModel)
	{
		pModel=FindModel(sFileName);
		if(pModel){return pModel;}
		pModel=new CModel;
	}
	else
	{
		pModel->Reset();
	}

	bool bLoaded=false;
	if(strcasecmp(sExt,".ASE")==0)	{bLoaded=LoadASE(pModel,sFileName);}
	else
	{
	  RTTRACE("Tipo de extension de modelo desconocida: %s",sFileName.c_str());
	}
	if(bLoaded)
	{
		if(!pExistingModel)
		{
			AddModel(pModel);
		}
		//_MRT("Loaded Model %s, %d polygons",sFileName.c_str(),pModel->m_pAnimations[0].m_pFrames[0].m_nPolygons);
	}
	else
	{
		if(!pExistingModel)
		{
			delete pModel;
		}
		pModel=NULL;
	}
	return pModel;
}

bool CResourceStore::LoadASE(CModel *pModel,string sFileName)
{
	bool bResult=false;

	CASEFileType	fileASE;
	C3DSFileTypeBase *pFile=&fileASE;
	if(pFile->Open(sFileName.c_str()))
	{
		bResult=true;
		CModelAnimation	*pAnimations=new CModelAnimation[1];
		CFrame		    *pFrames	=new CFrame[pFile->m_Scene.nFrameCount];

		pModel->m_sName=sFileName;
		pModel->m_nAnimations=1;
		pModel->m_pAnimations=pAnimations;
		pModel->m_pAnimations[0].m_nFrames=pFile->m_Scene.nFrameCount;
		pModel->m_pAnimations[0].m_pFrames=pFrames;

		map<string,CMaterial *> mTranslatedMaterials;
		map<string,S3DSMaterial *> m3DSMaterials;
		for(unsigned int m=0;m<pFile->m_vMaterials.size();m++)
		{
			S3DSMaterial *p3DSMaterial=pFile->m_vMaterials[m];
			string sName=p3DSMaterial->sName;
			CMaterial *pMaterial=new CMaterial;
			pMaterial->vAmbientColor=p3DSMaterial->vAmbientColor;
			pMaterial->vDiffuseColor=p3DSMaterial->vDiffuseColor;
			pMaterial->vSpecularColor=p3DSMaterial->vSpecularColor;
			pMaterial->fShininess=p3DSMaterial->fShininess;
			pMaterial->dwMaterialType=p3DSMaterial->dwMaterialType;
			pMaterial->fOpacity=1.0-p3DSMaterial->fTranparency;
			pMaterial->bTwoSided=p3DSMaterial->bTwoSided;
			pMaterial->sTexture=p3DSMaterial->sFile;

			CMaterial *pTest=FindMaterial(pMaterial);
			if(pTest)
			{delete pMaterial;pMaterial=pTest;}
			else
			{AddMaterial(pMaterial);}

			m3DSMaterials[sName]=p3DSMaterial;
			mTranslatedMaterials[sName]=pMaterial;
		}

		for(int f=0;f<pFile->m_Scene.nFrameCount;f++)
		{
			CFrame *pFrame=&pFrames[f];

			vector<S3DSObject *>::iterator i;
			for(i=pFile->m_vObjects.begin();i!=pFile->m_vObjects.end();i++)
			{
				S3DSObject *pObject=*i;
				if(pObject->bVisible)
				{
					pFrame->m_nPolygons+=pObject->vAnimationFrames[f]->nFaces;
				}
			}
			if(pFrame->m_nPolygons)
			{
				int nPol=0;
				pFrame->m_pPolygons=new CMaterialPolygon[pFrame->m_nPolygons];

				for(unsigned int x=0;x<pFile->m_vObjects.size();x++)
				{
					int nBasePol=nPol;
					S3DSObject *pObject=pFile->m_vObjects[x];
					S3DSFrame  *p3DSFrame=pObject->vAnimationFrames[f];

					//_MRT("Loaded Object %s, %d polygons",pObject->sName,p3DSFrame->nFaces);
					if(pObject->bVisible && p3DSFrame->nVertexes && p3DSFrame->nFaces)
					{
						for(int y=0;y<p3DSFrame->nFaces;y++)
						{
							pFrame->m_pPolygons[nPol].m_nVertexes=3;
							pFrame->m_pPolygons[nPol].m_pVertexes=new CVector[3];
							pFrame->m_pPolygons[nPol].m_pTextureCoords=new CVector[3];
							pFrame->m_pPolygons[nPol].m_pVertexNormals=new CVector[3];
							pFrame->m_pPolygons[nPol].m_pVertexes[0]=p3DSFrame->pVertexes[p3DSFrame->pFaces[y*3]];
							pFrame->m_pPolygons[nPol].m_pVertexes[1]=p3DSFrame->pVertexes[p3DSFrame->pFaces[y*3+1]];
							pFrame->m_pPolygons[nPol].m_pVertexes[2]=p3DSFrame->pVertexes[p3DSFrame->pFaces[y*3+2]];
							if(p3DSFrame->pColorVertexes)
							{
								pFrame->m_pPolygons[nPol].m_pVertexColors=new CVector[3];
								for(int z=0;z<p3DSFrame->nColorFaces;z++)
								{
									if(p3DSFrame->pColorFaces[z].nFaceIndex==y)
									{
										pFrame->m_pPolygons[nPol].m_pVertexColors[0]=p3DSFrame->pColorVertexes[p3DSFrame->pColorFaces[z].pColorVertexes[0]];
										pFrame->m_pPolygons[nPol].m_pVertexColors[1]=p3DSFrame->pColorVertexes[p3DSFrame->pColorFaces[z].pColorVertexes[1]];
										pFrame->m_pPolygons[nPol].m_pVertexColors[2]=p3DSFrame->pColorVertexes[p3DSFrame->pColorFaces[z].pColorVertexes[2]];
										break;
									}
								}
							}
							for(int z=0;z<p3DSFrame->nTextFaces;z++)
							{
								if(p3DSFrame->pTextFaces[z].nFaceIndex==y)
								{
									pFrame->m_pPolygons[nPol].m_pTextureCoords[0]=p3DSFrame->pTextVertexes[p3DSFrame->pTextFaces[z].pTextVertexes[0]];
									pFrame->m_pPolygons[nPol].m_pTextureCoords[1]=p3DSFrame->pTextVertexes[p3DSFrame->pTextFaces[z].pTextVertexes[1]];
									pFrame->m_pPolygons[nPol].m_pTextureCoords[2]=p3DSFrame->pTextVertexes[p3DSFrame->pTextFaces[z].pTextVertexes[2]];
									break;
								}
							}

							for(int a=0;a<3;a++)
							{
								if(p3DSFrame->pVertexNormals)
								{
									pFrame->m_pPolygons[nPol].m_pVertexNormals[a]=p3DSFrame->pVertexNormals[p3DSFrame->pFaces[y*3+a]];
								}
								else
								{
									CVector vNormal;
									for(int b=0;b<p3DSFrame->nFaces;b++)
									{
										if(p3DSFrame->pFaces[b*3]==p3DSFrame->pFaces[y*3+a] || p3DSFrame->pFaces[b*3+1]==p3DSFrame->pFaces[y*3+a] || p3DSFrame->pFaces[b*3+2]==p3DSFrame->pFaces[y*3+a])
										{
											CPlane plane(p3DSFrame->pVertexes[p3DSFrame->pFaces[b*3]],p3DSFrame->pVertexes[p3DSFrame->pFaces[b*3+1]],p3DSFrame->pVertexes[p3DSFrame->pFaces[b*3+2]]);
											vNormal+=(CVector)plane;
										}
									}
									vNormal.N();
									pFrame->m_pPolygons[nPol].m_pVertexNormals[a]=vNormal;
								}
							}
							nPol++;
						}
					}
					if(pObject->dwMaterialId!=(unsigned int)-1)
					{
						for(unsigned int y=0;y<p3DSFrame->sObjectMaterials.size();y++)
						{
							S3DSObjectMaterial	*pObjectMaterial=p3DSFrame->sObjectMaterials[y];
							S3DSMaterial		*p3DSMaterial=m3DSMaterials[pObjectMaterial->sName];
							CMaterial			*pMaterial=mTranslatedMaterials[pObjectMaterial->sName];
							for(int z=0;z<pObjectMaterial->nFaces;z++)
							{
								int nPolygonIndex=nBasePol+pObjectMaterial->pFaces[z];
								CMaterialPolygon *pPolygon=&pFrame->m_pPolygons[nPolygonIndex];
								pPolygon->m_pMaterial=pMaterial;
								for(unsigned int a=0;a<pFrame->m_pPolygons[nPolygonIndex].m_nVertexes;a++)
								{
									CVector vTextureCenter(0.5,0.5,0);
									CMatrix m;
									pPolygon->m_pTextureCoords[a].c[0]-=p3DSMaterial->fTextureUOffset;
									pPolygon->m_pTextureCoords[a].c[1]-=p3DSMaterial->fTextureVOffset;
									m.S(p3DSMaterial->fTextureUScale,p3DSMaterial->fTextureVScale,1,vTextureCenter);
									pPolygon->m_pTextureCoords[a]*=m;
									m.R(CVector(0,0,1),p3DSMaterial->fTextureAngle,vTextureCenter);
									pPolygon->m_pTextureCoords[a]*=m;
								}
							}
						}
					}
					else
					{
						CMaterial *pMaterial=new CMaterial;
						pMaterial->vAmbientColor=pObject->vWireframeColor;
						pMaterial->vDiffuseColor=pObject->vWireframeColor;
						pMaterial->vSpecularColor=pObject->vWireframeColor;
						pMaterial->dwMaterialType=MATERIAL_SOLID;
						pMaterial->fOpacity=1.0;

						CMaterial *pTest=FindMaterial(pMaterial);
						if(pTest)
							{delete pMaterial;pMaterial=pTest;}
						else
							{AddMaterial(pMaterial);}

						for(int a=0;a<p3DSFrame->nFaces;a++)
						{
							pFrame->m_pPolygons[nBasePol+a].m_pMaterial=pMaterial;
						}
					}
				}
			}
			pFrame->CalcBBox();
		}
	}
	return bResult;
}


void CResourceStore::AddModel(CModel *pModel)
{
	m_mModels[pModel->m_sName]=pModel;
}

void CResourceStore::FreeModels()
{
	map<string,CModel*>::iterator i;
	for(i=m_mModels.begin();i!=m_mModels.end();i++)
	{
		CModel *pModel=i->second;
		if(pModel){delete pModel;pModel=NULL;}
	}
	m_mModels.clear();
}

void CResourceStore::FreeMaterials()
{
	deque<CMaterial*>::iterator i;
	for(i=m_lMaterials.begin();i!=m_lMaterials.end();i++)
	{
		CMaterial *pMaterial=*i;
		if(pMaterial){delete pMaterial;pMaterial=NULL;}
	}
	m_lMaterials.clear();
}

void CResourceStore::Destroy()
{
	FreeAllResources();
}

void CResourceStore::FreeAllResources()
{
	FreeModels();
	FreeMaterials();
}

CMaterial *CResourceStore::FindMaterial(CMaterial *pMaterial)
{
	deque<CMaterial*>::iterator i;
	for(i=m_lMaterials.begin();i!=m_lMaterials.end();i++)
	{
		CMaterial *pExisting=*i;
		if(*pExisting==*pMaterial){return pExisting;}
	}
	return NULL;
}

CModel *CResourceStore::FindModel(string sName)
{
	map<string,CModel*>::iterator i;
	i=m_mModels.find(sName);
	if(i!=m_mModels.end())
	{
		CModel *pModel=i->second;
		return pModel;
	}
	return NULL;
}

void CResourceStore::AddMaterial(CMaterial *pMaterial)
{
	m_lMaterials.push_back(pMaterial);
}

void CResourceStore::Init()
{
}
