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


#include "./stdafx.h"
#include "GCMFiles.h"
#include "OBJFiles.h"
#include "OpenGLModel.h"

COpenGLModel::COpenGLModel(void)
{
	m_bLoadPending=false;
	m_bLoadResult=false;
	m_bLoadBSP=false;
	m_pModelBSP=NULL;
}

COpenGLModel::~COpenGLModel(void)
{
	RemoveAnimations();
}

std::string	 COpenGLModel::GetFileName(){return m_sFileName;}


bool COpenGLModel::LoadFromFile()
{
	m_bLoadPending=false;
	m_bLoadResult=false;
	RemoveAnimations();
	
	int nStartTime=GetTimeStamp();

	// Try to load the model as a compiled one.
	char sGCMFile[MAX_PATH]={0};
	strcpy(sGCMFile,m_sFileName.c_str());
	ReplaceExtension(sGCMFile,".gcm");
	
	std::string sFileName=sGCMFile;
	char pPath[MAX_PATH];
	GetFileFolder(sFileName.c_str(),pPath);
	
	bool bGCMOutdated=false;
	bool bLoadedGCM=false;
	CGCMFileType gcmfile;
	
	if( afexists(sFileName.c_str()) &&
		afexists(sGCMFile))
	{
		time_t asetime=GetFileTimeStamp(m_sFileName.c_str());
		time_t gcmfile=GetFileTimeStamp(sGCMFile);
		if(gcmfile<asetime)
		{
			bGCMOutdated=true;
			RTTRACE("COpenGLModel::LoadFromFile -> GCM file for %s is out of date, rebuilding",m_sFileName.c_str());
		}
	}

	if(!bGCMOutdated)
	{
		bLoadedGCM=gcmfile.Open(sGCMFile);
		if(!bLoadedGCM){RTTRACE("COpenGLModel::LoadFromFile -> GCM file for %s not found, rebuilding if possible",m_sFileName.c_str());}
	}
	
	char sExt[MAX_PATH]={0};
	if(!bLoadedGCM)
	{
		GetExtension(m_sFileName.c_str(),sExt);
		if(strcasecmp(sExt,".OBJ")==0)
		{
			COBJFileType	objfile;
			if(!objfile.Open(m_sFileName.c_str()))
			{
				RTTRACE("COpenGLModel::LoadFromFile -> Failed to load model %s",m_sFileName.c_str());
				return false;
			}
			objfile.ToGCM(&gcmfile);
		}
		else
		{
			CASEFileType	asefile;
			if(!asefile.Open(m_sFileName.c_str()))
			{
				RTTRACE("COpenGLModel::LoadFromFile -> Failed to load model %s",m_sFileName.c_str());
				return false;
			}
			asefile.ToGCM(&gcmfile);
		}
		if(!gcmfile.Save(sGCMFile))
		{
			RTTRACE("COpenGLModel::LoadFromFile -> Failed to save GCM for model %s",m_sFileName.c_str());
		}
	}
	
	SModelAnimation *pAnimation=new SModelAnimation;
	m_vAnimations.push_back(pAnimation);
	
	for (unsigned int f=0;f<gcmfile.GetFrames();f++)
	{
		SModelFrame *pFrame=new SModelFrame;
		pAnimation->vFrames.push_back(pFrame);

		for (unsigned int b=0;b<gcmfile.GetFrameBuffers(f);b++)
		{
			SModelRenderBuffer *pBuffer=new SModelRenderBuffer;
			gcmfile.GetBufferMaterial(f,b,&pBuffer->vAmbientColor,&pBuffer->vDiffuseColor,&pBuffer->vSpecularColor,&pBuffer->fShininess,&pBuffer->fOpacity);
						
			unsigned long nVertexes=0;
			unsigned long nFaces=0;
			unsigned long nTextureLevels=0;

			float *pVertexArray=NULL;
			float *pNormalArray=NULL;
			float *pNormalMapArray=NULL;
			float *pColorArray=NULL;
			unsigned int *pFaceVertexIndexes=NULL;
			std::string sNormalMap;
			
			gcmfile.GetBufferVertexes(f,b,&nVertexes,&pVertexArray);
			gcmfile.GetBufferFaces(f,b,&nFaces,&pFaceVertexIndexes);
			gcmfile.GetBufferColors(f,b,&pColorArray);
			gcmfile.GetBufferNormals(f,b,&pNormalArray);
			gcmfile.GetBufferNormalMap(f,b,&sNormalMap);
			gcmfile.GetBufferNormalMapCoords(f,b,&pNormalMapArray);
			gcmfile.GetBufferTextureLevels(f,b,&nTextureLevels);
			
			pBuffer->nVertexes=nVertexes;
			pBuffer->nFaces=nFaces;
			pBuffer->pVertexArray=new GLfloat[nVertexes*3];
			pBuffer->pFaceVertexIndexes=new GLuint[nFaces*3];
			
			memcpy(pBuffer->pVertexArray,pVertexArray,sizeof(GLfloat)*nVertexes*3);
			memcpy(pBuffer->pFaceVertexIndexes,pFaceVertexIndexes,sizeof(GLuint)*nFaces*3);
			
			if(pColorArray)
			{
				pBuffer->pColorArray=new GLfloat[nVertexes*4];
				memcpy(pBuffer->pColorArray,pColorArray,sizeof(GLfloat)*nVertexes*4);
			}
			if(pNormalArray)
			{
				pBuffer->pNormalArray=new GLfloat[nVertexes*3];
				memcpy(pBuffer->pNormalArray,pNormalArray,sizeof(GLfloat)*nVertexes*3);
			}
			if(pNormalMapArray)
			{
				if(!pBuffer->normalMap.Attach(m_piSystem,sNormalMap,false))
				{
					pBuffer->normalMap.Create(m_piSystem,"Texture",sNormalMap);
					if (pBuffer->normalMap.m_piTexture)
					{
						pBuffer->normalMap.m_piTexture->Load(sNormalMap,NULL,NULL,1.0);
					}
				}
				
				pBuffer->pNormalMapArray=new GLfloat[nVertexes*2];
				memcpy(pBuffer->pNormalMapArray,pNormalMapArray,sizeof(GLfloat)*nVertexes*2);
			}
			
			for(unsigned int l=0;l<nTextureLevels;l++)
			{
				std::string sTexture;
				GLfloat *pTexVertexArray=NULL;
				gcmfile.GetBufferTextureCoords(f,b,l,&pTexVertexArray);
				gcmfile.GetBufferTexture(f,b,l,&sTexture);
				if(pTexVertexArray)
				{
					SModelTextureLevel *pLevel=new SModelTextureLevel;
					pLevel->pTexVertexArray=new GLfloat[nVertexes*2];
					memcpy(pLevel->pTexVertexArray,pTexVertexArray,sizeof(GLfloat)*pBuffer->nVertexes*2);
					
					if(!pLevel->texture.Attach(m_piSystem,sTexture,false))
					{
						pLevel->texture.Create(m_piSystem,"Texture",sTexture);
						if (pLevel->texture.m_piTexture)
						{
							pLevel->texture.m_piTexture->Load(sTexture,NULL,NULL,pBuffer->fOpacity);
						}
					}
					pBuffer->vTextureLevels.push_back(pLevel);
				}
			}			
			if (pBuffer->nVertexes)
			{
				pFrame->vRenderBuffers.push_back(pBuffer);
			}
			else
			{
				delete pBuffer;
			}
		}
	}
	
	if(m_bLoadBSP){LoadBSP(sFileName.c_str());}
	UpdateBufferObjects();
	
	m_bLoadResult=true;
	
	RTTRACE("COpenGLModel::LoadFromFile -> Loaded model %s (%d ms)",m_sFileName.c_str(),GetTimeStamp()-nStartTime);
	return true;
}

void COpenGLModel::GetGeometry(std::vector<CPolygon*> *pPolygons)
{
	if(m_bLoadPending){LoadFromFile();}
	
  	if(m_vAnimations.size()==0){return;}
	if(m_vAnimations[0]->vFrames.size()==0){return;}
	
	SModelFrame *pFrame=m_vAnimations[0]->vFrames[0];
	for(unsigned int b=0;b<pFrame->vRenderBuffers.size();b++)
	{
	  SModelRenderBuffer *pBuffer=pFrame->vRenderBuffers[b];
	  GLuint *pFaceCursor=pBuffer->pFaceVertexIndexes;
	  for(int f=0;f<pBuffer->nFaces;f++)
	  {
		CPolygon *pPolygon=new CPolygon;
		pPolygon->m_nVertexes=3;
		pPolygon->m_pVertexes=new CVector[3];
		for(unsigned int v=0;v<3;v++)
		{
		  int nVertexIndex=(*pFaceCursor++);
		  pPolygon->m_pVertexes[v].c[0]=pBuffer->pVertexArray[nVertexIndex*3];
		  pPolygon->m_pVertexes[v].c[1]=pBuffer->pVertexArray[nVertexIndex*3+1];
		  pPolygon->m_pVertexes[v].c[2]=pBuffer->pVertexArray[nVertexIndex*3+2];
		}
		
		pPolygon->CalcPlane();
		pPolygons->push_back(pPolygon);
	  }
	}
}

bool COpenGLModel::LoadBSP(const char *pFileName)
{
	CBSPNode *pModelBSP=NULL;;
	char sGBSFile[MAX_PATH]={0};
	char sASEFile[MAX_PATH]={0};
	strcpy(sGBSFile,pFileName);
	strcpy(sASEFile,pFileName);
	ReplaceExtension(sGBSFile,".gbs");
	ReplaceExtension(sASEFile,".ase");
	
	
	bool bGBSOutdated = !afexists(sGBSFile);
	if(bGBSOutdated)
	{
		RTTRACE("COpenGLModel::LoadBSP -> GBS file for %s does not exist, rebuilding",m_sFileName.c_str());
	}
	if(!bGBSOutdated &&  afexists(sASEFile))
	{
		time_t asetime=GetFileTimeStamp(sASEFile);
		time_t gbstime=GetFileTimeStamp(sGBSFile);
		if(gbstime<asetime)
		{
			bGBSOutdated=true;
			RTTRACE("COpenGLModel::LoadBSP -> GBS file for %s is out of date, rebuilding",m_sFileName.c_str());
		}
	}
	
	if(bGBSOutdated)
	{
		std::vector<CPolygon*> vGeometry;
		CGBSFileType gbsFile;
		GetGeometry(&vGeometry);
		pModelBSP=BSPFromPolygonVector(NULL,1,&vGeometry,CONTENT_NODE,NULL,true);
		if(pModelBSP){gbsFile.Save(sGBSFile,pModelBSP,NULL);}
		for(unsigned int x=0;x<vGeometry.size();x++){delete vGeometry[x];}
		vGeometry.clear();
	}
	else
	{
		CGBSFileType gbsFile;
		if(!gbsFile.Load(sGBSFile,&pModelBSP,NULL))
		{
			RTTRACE("COpenGLModel::LoadBSP -> Failed to load BSP file for %s",m_sFileName.c_str());
		}
	}

	m_pModelBSP=pModelBSP;
	return (m_pModelBSP!=NULL);
}

bool COpenGLModel::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	m_bLoadPending=(bResult && m_sFileName.length());
	return bResult;
}

bool COpenGLModel::Load( std::string sFileName )
{
	m_sFileName=sFileName;
	return LoadFromFile();
}

void COpenGLModel::Create()
{
	m_bLoadBSP=false;
	RemoveAnimations();
}

void COpenGLModel::Render(IGenericRender *piRender,unsigned long nAnimation,unsigned long nFrame)
{
	if(m_bLoadPending){LoadFromFile();}
	if(nAnimation>=m_vAnimations.size()){return;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return;}
	piRender->RenderModel(Origin,Origin,this,nAnimation,nFrame);
}

void COpenGLModel::UpdateBufferObjects()
{
	bool bStrided=false;

	for(unsigned long x=0;x<m_vAnimations.size();x++)
	{
		SModelAnimation *pAnimation=m_vAnimations[x];
		for(unsigned long y=0;y<pAnimation->vFrames.size();y++)
		{
			bool bFirstVertex=true;
			SModelFrame *pFrame=pAnimation->vFrames[y];
			for(unsigned long nBuffer=0;nBuffer<pFrame->vRenderBuffers.size();nBuffer++)
			{
				SModelRenderBuffer *pBuffer=pFrame->vRenderBuffers[nBuffer];

				UpdateTangentBasis(pBuffer);

				// Compute BBox
				float *pVertexCursor=pBuffer->pVertexArray;
				for(int v=0;v<pBuffer->nVertexes;v++)
				{
					for(unsigned long c=0;c<3;c++,pVertexCursor++)
					{
						if(bFirstVertex || *pVertexCursor<pFrame->vMins.c[c]){pFrame->vMins.c[c]=*pVertexCursor;}
						if(bFirstVertex || *pVertexCursor>pFrame->vMaxs.c[c]){pFrame->vMaxs.c[c]=*pVertexCursor;}
					}
					bFirstVertex=false;
				}

				// Buffer objects
				
				if(pBuffer->nBufferObject!=0){glDeleteBuffers(1,&pBuffer->nBufferObject);pBuffer->nBufferObject=0;}
				if(pBuffer->nIndexesBufferObject!=0){glDeleteBuffers(1,&pBuffer->nIndexesBufferObject);pBuffer->nIndexesBufferObject=0;}
#ifndef ANDROID
				if(pBuffer->nVertexArrayObject!=0){glDeleteVertexArrays(1,&pBuffer->nVertexArrayObject);pBuffer->nVertexArrayObject=0;}
#endif
				int nDataPerVertex=0;
				unsigned int nOffset=0;
				if(bStrided)
				{
					if(pBuffer->pVertexArray){pBuffer->nVertexOffset=nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					if(pBuffer->pNormalArray){pBuffer->nNormalOffset=nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					if(pBuffer->pColorArray){pBuffer->nColorOffset=nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=4;}
					if(pBuffer->pNormalMapArray){pBuffer->nNormalMapOffset=nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=2;}
					if(pBuffer->pTangentArray){pBuffer->nTangentOffset=nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					if(pBuffer->pBitangentArray){pBuffer->nBitangentOffset=nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
					{
						SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTexture];
						//&& pTextureLevel->texture.m_piTexture 
						pBuffer->pTexOffset[nTexture]=nDataPerVertex*sizeof(GLfloat);
						if(pTextureLevel && pTextureLevel->pTexVertexArray){nDataPerVertex+=2;}
					}
					pBuffer->nVertexStride=nDataPerVertex*sizeof(GLfloat);
					pBuffer->nNormalStride=nDataPerVertex*sizeof(GLfloat);
					pBuffer->nColorStride=nDataPerVertex*sizeof(GLfloat);
					pBuffer->nTangentStride=nDataPerVertex*sizeof(GLfloat);
					pBuffer->nBitangentStride=nDataPerVertex*sizeof(GLfloat);
					pBuffer->nNormalMapStride=nDataPerVertex*sizeof(GLfloat);

					for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
					{
						pBuffer->pTexStride[nTexture]=nDataPerVertex*sizeof(GLfloat);
					}
				}
				else
				{
					if(pBuffer->pVertexArray){pBuffer->nVertexOffset=pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					if(pBuffer->pNormalArray){pBuffer->nNormalOffset=pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					if(pBuffer->pColorArray){pBuffer->nColorOffset=pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=4;}
					if(pBuffer->pNormalMapArray){pBuffer->nNormalMapOffset=pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=2;}
					if(pBuffer->pTangentArray){pBuffer->nTangentOffset=pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					if(pBuffer->pBitangentArray){pBuffer->nBitangentOffset=pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat);nDataPerVertex+=3;}
					
					for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
					{
						SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTexture];
						if(pTextureLevel && pTextureLevel->pTexVertexArray)
						{
							pBuffer->pTexOffset[nTexture]=pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat);
							nDataPerVertex+=2;
						}
					}
				}
				#ifndef ANDROID
				glGenVertexArrays(1,&pBuffer->nVertexArrayObject);
				pBuffer->bVertexArrayConfigured=false;
				if(pBuffer->nVertexArrayObject==0)
				{
					RTTRACE("COpenGLModel::UpdateBufferObjects -> Failed to create vertex array object");
				}
				#endif
				
				// Generacion del buffer object
				glGenBuffers(1,&pBuffer->nBufferObject);
				int nError=glGetError();
				if(pBuffer->nBufferObject)
				{
					glBindBuffer(GL_ARRAY_BUFFER,pBuffer->nBufferObject);
					GLfloat *pBufferObject = new GLfloat[pBuffer->nVertexes*nDataPerVertex];

					if(bStrided)
					{
						GLfloat *pTempVertex=pBuffer->pVertexArray;
						GLfloat *pTempNormal=pBuffer->pNormalArray;
						GLfloat *pTempColor=pBuffer->pColorArray;
						GLfloat *pTempNormalMap=pBuffer->pNormalMapArray;
						GLfloat *pTempTangent=pBuffer->pTangentArray;
						GLfloat *pTempBitangent=pBuffer->pBitangentArray;
						GLfloat **pTempTex=new float*[pBuffer->vTextureLevels.size()];
						GLfloat *pDest=pBufferObject;

						for(unsigned int x=0;x<pBuffer->vTextureLevels.size();x++)
						{
							pTempTex[x]=pBuffer->vTextureLevels[x]->pTexVertexArray;
						}

						for(unsigned int x=0;x<pBuffer->nVertexes;x++)
						{
							unsigned int dwOffset=0;

							if(pBuffer->pVertexArray){*pDest++=*pTempVertex++;*pDest++=*pTempVertex++;*pDest++=*pTempVertex++;}
							if(pBuffer->pNormalArray){*pDest++=*pTempNormal++;*pDest++=*pTempNormal++;*pDest++=*pTempNormal++;}
							if(pBuffer->pColorArray){*pDest++=*pTempColor++;*pDest++=*pTempColor++;*pDest++=*pTempColor++;*pDest++=*pTempColor++;}
							if(pBuffer->pNormalMapArray){*pDest++=*pTempNormalMap++;*pDest++=*pTempNormalMap++;*pDest++=*pTempNormalMap++;}
							if(pBuffer->pTangentArray){*pDest++=*pTempTangent++;*pDest++=*pTempTangent++;*pDest++=*pTempTangent++;}
							if(pBuffer->pBitangentArray){*pDest++=*pTempBitangent++;*pDest++=*pTempBitangent++;*pDest++=*pTempBitangent++;}
							for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
							{
								SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTexture];
								if(pTextureLevel && pTextureLevel->pTexVertexArray){*pDest++=*pTempTex[nTexture]++;*pDest++=*pTempTex[nTexture]++;}
							}

						}
						delete [] pTempTex;
						pTempTex=NULL;
					}
					else
					{
						if(pBuffer->pVertexArray){memcpy(((unsigned char*)pBufferObject)+pBuffer->nVertexOffset,pBuffer->pVertexArray,pBuffer->nVertexes*3*sizeof(GLfloat));}
						if(pBuffer->pNormalArray){memcpy(((unsigned char*)pBufferObject)+pBuffer->nNormalOffset,pBuffer->pNormalArray,pBuffer->nVertexes*3*sizeof(GLfloat));}
						if(pBuffer->pColorArray){memcpy(((unsigned char*)pBufferObject)+pBuffer->nColorOffset,pBuffer->pColorArray,pBuffer->nVertexes*4*sizeof(GLfloat));}
						if(pBuffer->pNormalMapArray){memcpy(((unsigned char*)pBufferObject)+pBuffer->nNormalMapOffset,pBuffer->pNormalMapArray,pBuffer->nVertexes*2*sizeof(GLfloat));}
						if(pBuffer->pTangentArray){memcpy(((unsigned char*)pBufferObject)+pBuffer->nTangentOffset,pBuffer->pTangentArray,pBuffer->nVertexes*3*sizeof(GLfloat));}
						if(pBuffer->pBitangentArray){memcpy(((unsigned char*)pBufferObject)+pBuffer->nBitangentOffset,pBuffer->pBitangentArray,pBuffer->nVertexes*3*sizeof(GLfloat));}

						for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
						{
							SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTexture];
							if(pTextureLevel && pTextureLevel->pTexVertexArray)
							{
								memcpy(((unsigned char*)pBufferObject)+pBuffer->pTexOffset[nTexture],pTextureLevel->pTexVertexArray,pBuffer->nVertexes*2*sizeof(GLfloat));
							}
						}
					}

					glBufferData(GL_ARRAY_BUFFER,pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat),pBufferObject,GL_STATIC_DRAW);
					if((nError=glGetError())!=GL_NO_ERROR){RTTRACE("COpenGLModel::UpdateBufferObjects -> Error setting buffer size %d",nError);}
					delete [] pBufferObject;
					pBufferObject=NULL;

					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				else
				{
					RTTRACE("COpenGLModel::UpdateBufferObjects -> Failed to create buffer object");
				}
				
				glGenBuffers(1,&pBuffer->nIndexesBufferObject);
				nError=glGetError();
				if(pBuffer->nIndexesBufferObject)
				{
					GLushort *puShort=new GLushort[pBuffer->nFaces*3];
					for(int v=0;v<pBuffer->nFaces*3;v++){puShort[v]=pBuffer->pFaceVertexIndexes[v];}
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->nIndexesBufferObject);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, pBuffer->nFaces*3*sizeof(GLushort),puShort,GL_STATIC_DRAW);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					delete [] puShort;
				}
				else
				{
					RTTRACE("COpenGLModel::UpdateBufferObjects -> Failed to create index buffer object");
				}
			}
			pFrame->vSize=(pFrame->vMaxs-pFrame->vMins);
			pFrame->dRadius=GetBBoxRadius(pFrame->vMins,pFrame->vMaxs);
		}
	}
}

unsigned long COpenGLModel::GetAnimations()
{
	if(m_bLoadPending){return LoadFromFile();}
	return m_vAnimations.size();
}

unsigned long COpenGLModel::GetAnimationFrames(unsigned long nAnimation)
{
	if(m_bLoadPending){return LoadFromFile();}
	if(nAnimation>=m_vAnimations.size()){return 0;}
	return m_vAnimations[nAnimation]->vFrames.size();
}	

unsigned long COpenGLModel::GetFrameRenderBuffers( unsigned long nAnimation,unsigned long nFrame )
{
	if(m_bLoadPending){return LoadFromFile();}
	if(nAnimation>=m_vAnimations.size()){return 0;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return 0;}
	return m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.size();
}

void COpenGLModel::GetFrameBBox(unsigned long nAnimation,unsigned long nFrame,CVector *pvMins,CVector *pvMaxs)
{
	if(m_bLoadPending){LoadFromFile();}
	if(nAnimation>=m_vAnimations.size()){return;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return;}
	*pvMins=m_vAnimations[nAnimation]->vFrames[nFrame]->vMins;
	*pvMaxs=m_vAnimations[nAnimation]->vFrames[nFrame]->vMaxs;
}

CVector COpenGLModel::GetFrameSize( unsigned long nAnimation,unsigned long nFrame )
{
	if(m_bLoadPending){LoadFromFile();}
	if(nAnimation>=m_vAnimations.size()){return Origin;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return Origin;}
	return m_vAnimations[nAnimation]->vFrames[nFrame]->vSize;
}

double COpenGLModel::GetFrameRadius( unsigned long nAnimation,unsigned long nFrame )
{
	if(m_bLoadPending){LoadFromFile();}
	if(nAnimation>=m_vAnimations.size()){return 0;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return 0;}
	return m_vAnimations[nAnimation]->vFrames[nFrame]->dRadius;
}


unsigned long COpenGLModel::AddAnimation()
{
	SModelAnimation *pAnimation=new SModelAnimation;
	m_vAnimations.push_back(pAnimation);
	return (m_vAnimations.size()-1);
}

unsigned long COpenGLModel::AddAnimationFrame( unsigned long nAnimation)
{
	if(nAnimation>=m_vAnimations.size()){return 0;}
	SModelFrame *pFrame=new SModelFrame;
	m_vAnimations[nAnimation]->vFrames.push_back(pFrame);
	return (m_vAnimations[nAnimation]->vFrames.size()-1);
}

unsigned long COpenGLModel::AddRenderBuffer( unsigned long nAnimation,unsigned long nFrame )
{
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return 0;}
	SModelRenderBuffer *pRenderBuffer=new SModelRenderBuffer;
	m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.push_back(pRenderBuffer);
	return (m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.size()-1);
}

SModelRenderBuffer *COpenGLModel::GetRenderBuffer( unsigned long nAnimation, unsigned long nFrame, unsigned long nBuffer )
{
	if(m_bLoadPending){LoadFromFile();}
	if(nAnimation>=m_vAnimations.size()){return NULL;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return NULL;}
	if(nBuffer>=m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.size()){return NULL;}
	return m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers[nBuffer];
}

void COpenGLModel::SetRenderBufferMaterial( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector vAmbientColor,CVector vDiffuseColor,CVector vSpecularColor, float fShininess, float fOpacity )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	pBuffer->vAmbientColor=vAmbientColor;
	pBuffer->vDiffuseColor=vDiffuseColor;
	pBuffer->vSpecularColor=vSpecularColor;
	pBuffer->fOpacity=fOpacity;
	pBuffer->fShininess=fShininess;
}

void COpenGLModel::SetRenderBufferTexture( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture *piTexture )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	while(nTextureLevel>=pBuffer->vTextureLevels.size()){pBuffer->vTextureLevels.push_back(new SModelTextureLevel);}
	SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTextureLevel];
	pTextureLevel->texture.Attach(piTexture);
}

void COpenGLModel::SetRenderBufferTextureMatrix(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,CMatrix *pMatrix)
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	while(nTextureLevel>=pBuffer->vTextureLevels.size()){pBuffer->vTextureLevels.push_back(new SModelTextureLevel);}
	SModelTextureLevel *pTextureLevel= pBuffer->vTextureLevels[nTextureLevel];
	if(pMatrix)
	{
		pTextureLevel->texMatrix=*pMatrix;
		pTextureLevel->bTextMatrixIdentity=false;
	}
	else
	{
		pTextureLevel->texMatrix.I();
		pTextureLevel->bTextMatrixIdentity=true;
	}
}

void COpenGLModel::SetRenderBufferVertexes( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nVertexes,float *pVertexes )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pVertexArray;
	pBuffer->nVertexes=nVertexes;
	pBuffer->pVertexArray=pVertexes;
	ClearTangentBasis(pBuffer);
}

void COpenGLModel::SetRenderBufferFaces( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pFaceVertexIndexes;
	pBuffer->nFaces=nFaces;
	pBuffer->pFaceVertexIndexes=pFacesVertexes;
	ClearTangentBasis(pBuffer);
}

void COpenGLModel::SetRenderBufferNormals( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormals )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pNormalArray;
	pBuffer->pNormalArray=pNormals;
	ClearTangentBasis(pBuffer);
}

void COpenGLModel::SetRenderBufferColors( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pColors )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pColorArray;
	pBuffer->pColorArray=pColors;
}

void COpenGLModel::SetRenderBufferTextureCoords( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float *pTexVertexes )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	while(nTextureLevel>=pBuffer->vTextureLevels.size()){pBuffer->vTextureLevels.push_back(new SModelTextureLevel);}
	SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTextureLevel];
	delete [] pTextureLevel->pTexVertexArray;
	pTextureLevel->pTexVertexArray=pTexVertexes;
}

void COpenGLModel::SetRenderBufferNormalMap( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,IGenericTexture *piTexture )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	pBuffer->normalMap.Attach(piTexture);
	ClearTangentBasis(pBuffer);
}

void COpenGLModel::SetRenderBufferNormalMapCoords( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormalMapVertexes)
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pNormalMapArray;
	pBuffer->pNormalMapArray=pNormalMapVertexes;
	ClearTangentBasis(pBuffer);
}

void COpenGLModel::GetRenderBufferMaterial( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor, float *pfShininess, float *pfOpacity )
{
	if(m_bLoadPending){LoadFromFile();}
	if(pvAmbientColor){*pvAmbientColor=Origin;}
	if(pvDiffuseColor){*pvDiffuseColor=Origin;}
	if(pvSpecularColor){*pvSpecularColor=Origin;}
	if(pfShininess){*pfShininess=0;}
	if(pfOpacity){*pfOpacity=1.0;}

	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}

	if(pvAmbientColor){*pvAmbientColor=pBuffer->vAmbientColor;}
	if(pvDiffuseColor){*pvDiffuseColor=pBuffer->vDiffuseColor;}
	if(pvSpecularColor){*pvSpecularColor=pBuffer->vSpecularColor;}
	if(pfShininess){*pfShininess=pBuffer->fShininess;}
	if(pfOpacity){*pfOpacity=pBuffer->fOpacity;}
}

void COpenGLModel::GetRenderBufferTexture( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture **ppiTexture )
{
	if(m_bLoadPending){LoadFromFile();}
	if(ppiTexture){*ppiTexture=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(nTextureLevel>=pBuffer->vTextureLevels.size()){return;}
	if(ppiTexture){*ppiTexture=ADD(pBuffer->vTextureLevels[nTextureLevel]->texture.m_piTexture);}
}

void COpenGLModel::GetRenderBufferNormalMap( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,IGenericTexture **ppiTexture )
{
	if(m_bLoadPending){LoadFromFile();}
	if(ppiTexture){*ppiTexture=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppiTexture){*ppiTexture=ADD(pBuffer->normalMap.m_piTexture);}
}

void COpenGLModel::GetRenderBufferTextureMatrix(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,CMatrix *pMatrix)
{
	if(m_bLoadPending){LoadFromFile();}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	while(nTextureLevel>=pBuffer->vTextureLevels.size()){pBuffer->vTextureLevels.push_back(new SModelTextureLevel);}
	SModelTextureLevel *pTextureLevel= pBuffer->vTextureLevels[nTextureLevel];
	*pMatrix=pTextureLevel->texMatrix;
}


void COpenGLModel::GetRenderBufferVertexes( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes )
{
	if(m_bLoadPending){LoadFromFile();}
	if(pVertexes){*pVertexes=0;}
	if(ppVertexes){*ppVertexes=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(pVertexes){*pVertexes=pBuffer->nVertexes;}
	if(ppVertexes){*ppVertexes=pBuffer->pVertexArray;}
}

void COpenGLModel::GetRenderBufferFaces( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes )
{
	if(m_bLoadPending){LoadFromFile();}
	if(pFaces){*pFaces=0;}
	if(ppFacesVertexes){*ppFacesVertexes=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(pFaces){*pFaces=pBuffer->nFaces;}
	if(ppFacesVertexes){*ppFacesVertexes=pBuffer->pFaceVertexIndexes;}
}

void COpenGLModel::GetRenderBufferNormals( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormals )
{
	if(m_bLoadPending){LoadFromFile();}
	if(ppNormals){*ppNormals=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppNormals){*ppNormals=pBuffer->pNormalArray;}
}

void COpenGLModel::GetRenderBufferColors( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppColors )
{
	if(m_bLoadPending){LoadFromFile();}
	if(ppColors){*ppColors=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppColors){*ppColors=pBuffer->pColorArray;}
}

void COpenGLModel::GetRenderBufferTextureCoords( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes )
{
	if(m_bLoadPending){LoadFromFile();}
	if(ppTexVertexes){*ppTexVertexes=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(nTextureLevel>=pBuffer->vTextureLevels.size()){return;}
	if(ppTexVertexes){*ppTexVertexes=pBuffer->vTextureLevels[nTextureLevel]->pTexVertexArray;}
}

void COpenGLModel::GetRenderBufferNormalMapCoords( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormalMapVertexes )
{
	if(m_bLoadPending){LoadFromFile();}
	if(ppNormalMapVertexes){*ppNormalMapVertexes=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppNormalMapVertexes){*ppNormalMapVertexes=pBuffer->pNormalMapArray;}
}

void COpenGLModel::RemoveFrames( unsigned long nAnimation )
{
	if(nAnimation>=m_vAnimations.size()){return;}
	for(unsigned x=0;x<m_vAnimations[nAnimation]->vFrames.size();x++){delete m_vAnimations[nAnimation]->vFrames[x];}
	m_vAnimations[nAnimation]->vFrames.clear();
}

void COpenGLModel::RemoveAnimations()
{
	delete m_pModelBSP;
	m_pModelBSP=NULL;

	for(unsigned x=0;x<m_vAnimations.size();x++){delete m_vAnimations[x];}
	m_vAnimations.clear();
}

void COpenGLModel::RemoveRenderBuffers( unsigned long nAnimation,unsigned long nFrame )
{
	if(nAnimation>=m_vAnimations.size()){return;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return;}
	for(unsigned x=0;x<m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.size();x++){delete m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers[x];}
	m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.clear();
}

CBSPNode *COpenGLModel::GetBSP()
{
	if(m_bLoadPending){LoadFromFile();}
	return m_pModelBSP;
}

void COpenGLModel::SetBSPOptions(bool bLoad)
{
	m_bLoadBSP=bLoad;
}

void COpenGLModel::GetBSPOptions(bool *pbLoad)
{
	if(pbLoad){*pbLoad=m_bLoadBSP;}
}

void COpenGLModel::PrepareRenderBuffer(IOpenGLRender *piRender, unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer ,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)
{
	if(m_bLoadPending){LoadFromFile();}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation,nFrame,nBuffer);
	if(pBuffer==NULL){return;}
	
	bool bUseNormalMap=(!bRenderingShadow && pBuffer->pNormalMapArray && piRender->AreNormalMapsEnabled());
	
	int nOffset=0;
	SVertexBufferObject object;
	object.nVertexArrayObject=pBuffer->nVertexArrayObject;
	object.nBufferObject=pBuffer->nBufferObject;
	object.nIndexesBufferObject=pBuffer->nIndexesBufferObject;
	object.nVertexOffset=pBuffer->nVertexOffset;
	object.nVertexStride=pBuffer->nVertexStride;

	if(pBuffer->pNormalArray)
	{
		object.nNormalOffset=pBuffer->nNormalOffset;
		object.nNormalStride=pBuffer->nNormalStride;
	}
	if(pBuffer->pColorArray)
	{
		object.nColorOffset=pBuffer->nColorOffset;
		object.nColorStride=pBuffer->nColorStride;
	}
	if(pBuffer->pNormalMapArray)
	{
		object.nNormalMapOffset=pBuffer->nNormalMapOffset;
		object.nNormalMapStride=pBuffer->nNormalMapStride;
	}
	if(pBuffer->pTangentArray)
	{
		object.nTangentOffset=pBuffer->nTangentOffset;
		object.nTangentStride=pBuffer->nTangentStride;
	}
	if(pBuffer->pBitangentArray)
	{
		object.nBitangentOffset=pBuffer->nBitangentOffset;
		object.nBitangentStride=pBuffer->nBitangentStride;
	}

	if(piRender->AreTexturesEnabled())
	{
		object.nTextures=pBuffer->vTextureLevels.size();
		for(unsigned int x=0;x<pBuffer->vTextureLevels.size() && x<2;x++)
		{
			SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[x];
			if(pTextureLevel && pTextureLevel->texture.m_piTexture && pTextureLevel->pTexVertexArray)
			{
				object.pTexOffsets[x]=pBuffer->pTexOffset[x];
				object.pTexStrides[x]=pBuffer->pTexStride[x];
				if(!bRenderingShadow)
				{
					piRender->PrepareTexture(pBuffer->vTextureLevels[x]->texture.m_piTexture,x);
					if(!pBuffer->vTextureLevels[x]->bTextMatrixIdentity){piRender->SetTextureMatrix(&pBuffer->vTextureLevels[x]->texMatrix,x);}
				}
			}
		}
	}

#ifndef ANDROID
	if(pBuffer->nVertexArrayObject && !pBuffer->bVertexArrayConfigured)
	{
		pBuffer->bVertexArrayConfigured=true;
		glBindVertexArray(pBuffer->nVertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER,pBuffer->nBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pBuffer->nIndexesBufferObject);
		
		if(object.nVertexOffset!=-1)   {glEnableVertexAttribArray(pRenderMappings->nVertexAttribIndex);}
		if(object.nColorOffset!=-1)    {glEnableVertexAttribArray(pRenderMappings->nColorAttribIndex);}
		if(object.nNormalOffset!=-1)   {glEnableVertexAttribArray(pRenderMappings->nNormalAttribIndex);}
		if(object.nNormalMapOffset!=-1){glEnableVertexAttribArray(pRenderMappings->nNormalMapCoordAttribIndex);}
		if(object.nTangentOffset!=-1)  {glEnableVertexAttribArray(pRenderMappings->nTangentAttribIndex);}
		if(object.nBitangentOffset!=-1){glEnableVertexAttribArray(pRenderMappings->nBitangentAttribIndex);}
		
		if(object.nVertexOffset!=-1){glVertexAttribPointer(pRenderMappings->nVertexAttribIndex,3,GL_FLOAT,GL_FALSE,object.nVertexStride,(void*)object.nVertexOffset);}
		if(object.nColorOffset!=-1){glVertexAttribPointer(pRenderMappings->nColorAttribIndex,4,GL_FLOAT,GL_FALSE,object.nColorStride,(void*)object.nColorOffset);}
		if(object.nNormalOffset!=-1){glVertexAttribPointer(pRenderMappings->nNormalAttribIndex,3,GL_FLOAT,GL_FALSE,object.nNormalStride,(void*)object.nNormalOffset);}
		if(object.nNormalMapOffset!=-1){glVertexAttribPointer(pRenderMappings->nNormalMapCoordAttribIndex,2,GL_FLOAT,GL_FALSE,object.nNormalMapStride,(void*)object.nNormalMapOffset);}
		if(object.nTangentOffset!=-1){glVertexAttribPointer(pRenderMappings->nTangentAttribIndex,3,GL_FLOAT,GL_FALSE,object.nTangentStride,(void*)object.nTangentOffset);}
		if(object.nBitangentOffset!=-1){glVertexAttribPointer(pRenderMappings->nBitangentAttribIndex,3,GL_FLOAT,GL_FALSE,object.nBitangentStride,(void*)object.nBitangentOffset);}
		for(unsigned int x=0;x<2;x++)
		{
			if(object.pTexOffsets[x]!=-1)
			{
				glEnableVertexAttribArray(pRenderMappings->pTextureAttribIndex[x]);
				glVertexAttribPointer(pRenderMappings->pTextureAttribIndex[x],2,GL_FLOAT,GL_FALSE,object.pTexStrides[x],(void*)object.pTexOffsets[x]);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
	}
#endif
	piRender->SetVertexBufferObject(&object);
}


void COpenGLModel::CallRenderBuffer(IOpenGLRender *piRender, unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,unsigned int nInstances)
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation,nFrame,nBuffer);
	if(pBuffer==NULL){return;}

	if(pBuffer->nBufferObject)
	{
#ifndef ANDROID
		glDrawElementsInstancedARB(GL_TRIANGLES,pBuffer->nFaces*3,GL_UNSIGNED_SHORT,NULL,nInstances);
#else
		if(nInstances!=1){RTTRACE("COpenGLModel::CallRenderBuffer -> ERROR: GLES does not support object instancing !!!");}
		glDrawElements(GL_TRIANGLES,pBuffer->nFaces*3,GL_UNSIGNED_SHORT,NULL);
#endif
	}
	else
	{
		glDrawElements(GL_TRIANGLES,pBuffer->nFaces*3,GL_UNSIGNED_SHORT,pBuffer->pFaceVertexIndexes);
	}
}

void COpenGLModel::UnPrepareRenderBuffer(IOpenGLRender *piRender, unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer ,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings)
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation,nFrame,nBuffer);
	if(pBuffer==NULL){return;}
	piRender->SetVertexBufferObject(NULL);

	if(!bRenderingShadow && piRender->AreTexturesEnabled())
	{
		for(unsigned int x=0;x<pBuffer->vTextureLevels.size();x++)
		{
			if(pBuffer->vTextureLevels[x]->texture.m_piTexture && pBuffer->vTextureLevels[x]->pTexVertexArray)
			{
				piRender->UnprepareTexture(x);
			}
		}
	}
}

CTraceInfo COpenGLModel::GetTrace( const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	if(m_bLoadPending){LoadFromFile();}
	if(m_pModelBSP)
	{
		CVector vMins,vMaxs;
		GetFrameBBox(0,0,&vMins,&vMaxs);
		CTraceInfo info=m_pModelBSP->GetObjectTrace(p1,p2,vOrigin,vAngles,vMins,vMaxs);
		if(!info.m_bTraceHit)
		{
			info.m_dTraceFraction=1.0;
			info.m_vTracePos=p2;
		}
		return info;
	}
	else
	{
		CTraceInfo info;
		info.m_dTraceFraction=1.0;
		info.m_vTracePos=p2;
		return info;
	}
}

bool COpenGLModel::Prepare()
{
	if(m_bLoadPending){return LoadFromFile();}
	return m_bLoadResult;
}

void COpenGLModel::ClearTangentBasis(SModelRenderBuffer *pBuffer)
{
	delete [] pBuffer->pTangentArray;
	delete [] pBuffer->pBitangentArray;
	pBuffer->pTangentArray=NULL;
	pBuffer->pBitangentArray=NULL;
}

void COpenGLModel::UpdateTangentBasis(SModelRenderBuffer *pBuffer)
{
	ClearTangentBasis(pBuffer);
	
	if(!pBuffer->nVertexes || !pBuffer->pVertexArray){return;}
	if(!pBuffer->nFaces || !pBuffer->pFaceVertexIndexes){return;}
	if(!pBuffer->pNormalArray || !pBuffer->pNormalMapArray){return;}
	
	pBuffer->pTangentArray=new float[pBuffer->nVertexes*3];
	pBuffer->pBitangentArray=new float[pBuffer->nVertexes*3];
	
	ComputeTangentBasis(pBuffer->nVertexes,pBuffer->pVertexArray,pBuffer->nFaces,pBuffer->pFaceVertexIndexes,pBuffer->pNormalMapArray,pBuffer->pNormalArray,pBuffer->pTangentArray,pBuffer->pBitangentArray);
}

SModelTextureLevel::SModelTextureLevel()
{
	pTexVertexArray=NULL;
	bTextMatrixIdentity=true;
}

SModelTextureLevel::~SModelTextureLevel()
{
	delete [] pTexVertexArray;
	pTexVertexArray=NULL;
}

SModelRenderBuffer::SModelRenderBuffer()
{
	bVertexArrayConfigured=false;
	
	fShininess=0;
	fOpacity=1.0;
	
	nVertexArrayObject=0;
	nBufferObject=0;
	nIndexesBufferObject=0;

	nVertexes=0;
	nFaces=0;
	pVertexArray=NULL;
	pNormalArray=NULL;
	pNormalMapArray=NULL;
	pTangentArray=NULL;
	pBitangentArray=NULL;
	pColorArray=NULL;
	pFaceVertexIndexes=NULL;

	nVertexStride=0;
	nColorStride=0;
	nNormalStride=0;
	pTexStride[0]=pTexStride[1]=0;
	nNormalMapStride=0;
	nTangentStride=0;
	nBitangentStride=0;

	nVertexOffset=0;
	nColorOffset=0;
	nNormalOffset=0;
	pTexOffset[0]=pTexOffset[1]=0;
	nNormalMapOffset=0;
	nTangentOffset=0;
	nBitangentOffset=0;
}

SModelRenderBuffer::~SModelRenderBuffer()
{
	for(unsigned x=0;x<vTextureLevels.size();x++){delete vTextureLevels[x];}
	vTextureLevels.clear();

	if(nBufferObject!=0){glDeleteBuffers(1,&nBufferObject);nBufferObject=0;}
	if(nIndexesBufferObject!=0){glDeleteBuffers(1,&nIndexesBufferObject);nIndexesBufferObject=0;}
#ifndef ANDROID
	if(nVertexArrayObject!=0){glDeleteVertexArrays(1,&nVertexArrayObject);nVertexArrayObject=0;}
#endif
	delete [] pVertexArray;
	delete [] pNormalArray;
	delete [] pNormalMapArray;
	delete [] pTangentArray;
	delete [] pBitangentArray;
	delete [] pColorArray;
	delete [] pFaceVertexIndexes;

	pVertexArray=NULL;
	pNormalMapArray=NULL;
	pTangentArray=NULL;
	pBitangentArray=NULL;
	pNormalArray=NULL;
	pColorArray=NULL;
	pFaceVertexIndexes=NULL;
}

SModelFrame::SModelFrame()
{
	dRadius=0;
}

SModelFrame::~SModelFrame()
{
	for(unsigned x=0;x<vRenderBuffers.size();x++){delete vRenderBuffers[x];}
	vRenderBuffers.clear();
}

SModelAnimation::SModelAnimation(){}
SModelAnimation::~SModelAnimation()
{
	for(unsigned x=0;x<vFrames.size();x++){delete vFrames[x];}
	vFrames.clear();
}

