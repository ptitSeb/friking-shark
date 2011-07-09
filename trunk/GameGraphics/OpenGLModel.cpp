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
	
	if( FileExists(sFileName.c_str()) &&
		FileExists(sGCMFile))
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
	
	if(!bLoadedGCM)
	{
		CASEFileType	asefile;
		if(!asefile.Open(m_sFileName.c_str()))
		{
			RTTRACE("COpenGLModel::LoadFromFile -> Failed to load model %s",m_sFileName.c_str());
			return false;
		}
		asefile.ToGCM(&gcmfile);
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
			float *pColorArray=NULL;
			unsigned int *pFaceVertexIndexes=NULL;
			
			gcmfile.GetBufferVertexes(f,b,&nVertexes,&pVertexArray);
			gcmfile.GetBufferFaces(f,b,&nFaces,&pFaceVertexIndexes);
			gcmfile.GetBufferColors(f,b,&pColorArray);
			gcmfile.GetBufferNormals(f,b,&pNormalArray);
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
	UpdateFrameBuffers();
	
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
	
	
	bool bGBSOutdated = !FileExists(sGBSFile);
	if(bGBSOutdated)
	{
		RTTRACE("COpenGLModel::LoadBSP -> GBS file for %s does not exist, rebuilding",m_sFileName.c_str());
	}
	if(!bGBSOutdated &&  FileExists(sASEFile))
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
	SModelFrame *pFrame=m_vAnimations[nAnimation]->vFrames[nFrame];
	for(unsigned long x=0;x<pFrame->vRenderBuffers.size();x++)
	{
		PrepareRenderBuffer(piRender,nAnimation,nFrame,x,false);
		CallRenderBuffer(piRender,nAnimation,nFrame,x);
		UnPrepareRenderBuffer(piRender,nAnimation,nFrame,x,false);
	}
}

void COpenGLModel::UpdateFrameBuffers()
{
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

				int nDataPerVertex=0;
				if(pBuffer->pVertexArray){nDataPerVertex+=3;}
				if(pBuffer->pNormalArray){nDataPerVertex+=3;}
				if(pBuffer->pColorArray){nDataPerVertex+=4;}
				for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
				{
					SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTexture];
					if(pTextureLevel /*&& pTextureLevel->texture.m_piTexture */&& pTextureLevel->pTexVertexArray){nDataPerVertex+=2;}
				}

				// Generacion del buffer object
				glGenBuffers(1,&pBuffer->nBufferObject);
				int nError=glGetError();
				if(pBuffer->nBufferObject)
				{
					glBindBuffer(GL_ARRAY_BUFFER,pBuffer->nBufferObject);
					glBufferData(GL_ARRAY_BUFFER,pBuffer->nVertexes*nDataPerVertex*sizeof(GLfloat),NULL,GL_STATIC_DRAW);
					GLfloat *pBufferObject = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);

					unsigned int dwOffset=0;
					if(pBuffer->pVertexArray)
					{
						memcpy(((unsigned char*)pBufferObject)+dwOffset,pBuffer->pVertexArray,pBuffer->nVertexes*3*sizeof(GLfloat));
						dwOffset+=pBuffer->nVertexes*3*sizeof(GLfloat);
					}
					if(pBuffer->pNormalArray)
					{
						memcpy(((unsigned char*)pBufferObject)+dwOffset,pBuffer->pNormalArray,pBuffer->nVertexes*3*sizeof(GLfloat));
						dwOffset+=pBuffer->nVertexes*3*sizeof(GLfloat);
					}
					if(pBuffer->pColorArray)
					{
						memcpy(((unsigned char*)pBufferObject)+dwOffset,pBuffer->pColorArray,pBuffer->nVertexes*4*sizeof(GLfloat));
						dwOffset+=pBuffer->nVertexes*4*sizeof(GLfloat);
					}
					for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
					{
						SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTexture];
						if(pTextureLevel/* && pTextureLevel->texture.m_piTexture*/ && pTextureLevel->pTexVertexArray)
						{
							memcpy(((unsigned char*)pBufferObject)+dwOffset,pTextureLevel->pTexVertexArray,pBuffer->nVertexes*2*sizeof(GLfloat));
							dwOffset+=pBuffer->nVertexes*2*sizeof(GLfloat);
						}
					}
					glUnmapBuffer(GL_ARRAY_BUFFER);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				glGenBuffers(1,&pBuffer->nIndexesBufferObject);
				nError=glGetError();
				if(pBuffer->nIndexesBufferObject)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->nIndexesBufferObject);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, pBuffer->nFaces*3*sizeof(GLuint),NULL,GL_STATIC_DRAW);
					GLuint *pBufferObjectIndexes = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
					memcpy(pBufferObjectIndexes,pBuffer->pFaceVertexIndexes,sizeof(GLuint)*pBuffer->nFaces*3);
					glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
}

void COpenGLModel::SetRenderBufferFaces( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pFaceVertexIndexes;
	pBuffer->nFaces=nFaces;
	pBuffer->pFaceVertexIndexes=pFacesVertexes;
}

void COpenGLModel::SetRenderBufferNormals( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormals )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	delete [] pBuffer->pNormalArray;
	pBuffer->pNormalArray=pNormals;
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

void COpenGLModel::PrepareRenderBuffer(IGenericRender *piRender, unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer ,bool bRenderingShadow)
{
	if(m_bLoadPending){LoadFromFile();}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation,nFrame,nBuffer);
	if(pBuffer==NULL){return;}

	if(!bRenderingShadow && piRender->AreTexturesEnabled())
	{
		for(unsigned int x=0;x<pBuffer->vTextureLevels.size();x++)
		{
			if(pBuffer->vTextureLevels[x]->texture.m_piTexture && pBuffer->vTextureLevels[x]->pTexVertexArray)
			{
				piRender->SelectTexture(pBuffer->vTextureLevels[x]->texture.m_piTexture,x);
				if(!pBuffer->vTextureLevels[x]->bTextMatrixIdentity){piRender->SetTextureMatrix(&pBuffer->vTextureLevels[x]->texMatrix,x);}
			}
		}
	}
	if(pBuffer->nBufferObject)
	{
		glBindBuffer(GL_ARRAY_BUFFER,pBuffer->nBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pBuffer->nIndexesBufferObject);
	
		unsigned long nOffset=0;
		if(pBuffer->pVertexArray)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT,pBuffer->nVertexes*sizeof(GLfloat)*nOffset,NULL);
			nOffset+=3;
		}
		if(!bRenderingShadow)
		{
			GLfloat  pSpecular[]={(float)pBuffer->vSpecularColor.c[0],(float)pBuffer->vSpecularColor.c[1],(float)pBuffer->vSpecularColor.c[2],pBuffer->fOpacity};
			glMaterialfv(GL_FRONT,GL_SPECULAR,pSpecular);
			glMaterialf(GL_FRONT,GL_SHININESS,pBuffer->fShininess);

			if(pBuffer->pNormalArray)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT,0,(void*)(pBuffer->nVertexes*sizeof(GLfloat)*nOffset));
				nOffset+=3;
			}
			if(pBuffer->pColorArray)
			{
				glEnable(GL_COLOR_MATERIAL);
				glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(4,GL_FLOAT,0,(void*)(pBuffer->nVertexes*sizeof(GLfloat)*nOffset));
				nOffset+=4;
			}
			else
			{
				GLfloat  pAmbient[]={(float)pBuffer->vAmbientColor.c[0],(float)pBuffer->vAmbientColor.c[1],(float)pBuffer->vAmbientColor.c[2],pBuffer->fOpacity};
				GLfloat  pDiffuse[]={(float)pBuffer->vDiffuseColor.c[0],(float)pBuffer->vDiffuseColor.c[1],(float)pBuffer->vDiffuseColor.c[2],pBuffer->fOpacity};
				glMaterialfv(GL_FRONT,GL_AMBIENT,pAmbient);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,pDiffuse);
			}
			if(piRender->AreTexturesEnabled())
			{
				for(unsigned int x=0;x<pBuffer->vTextureLevels.size();x++)
				{
					SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[x];
					if(pTextureLevel && pTextureLevel->texture.m_piTexture && pTextureLevel->pTexVertexArray)
					{
						glClientActiveTextureARB(GL_TEXTURE0_ARB+x);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(2,GL_FLOAT,0,(void*)(pBuffer->nVertexes*sizeof(GLfloat)*nOffset));
						nOffset+=2;
					}
				}
			}
		}
	}
	else
	{
		if(pBuffer->pVertexArray)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT,0,pBuffer->pVertexArray);
		}
		if(!bRenderingShadow)
		{
			if(pBuffer->pNormalArray)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT,0,pBuffer->pNormalArray);
			}
			if(pBuffer->pColorArray)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(4,GL_FLOAT,0,pBuffer->pColorArray);

				glEnable(GL_COLOR_MATERIAL);
				glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
			}
			else
			{
				GLfloat  pAmbient[]={(float)pBuffer->vAmbientColor.c[0],(float)pBuffer->vAmbientColor.c[1],(float)pBuffer->vAmbientColor.c[2],pBuffer->fOpacity};
				GLfloat  pDiffuse[]={(float)pBuffer->vDiffuseColor.c[0],(float)pBuffer->vDiffuseColor.c[1],(float)pBuffer->vDiffuseColor.c[2],pBuffer->fOpacity};
				glMaterialfv(GL_FRONT,GL_AMBIENT,pAmbient);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,pDiffuse);
			}
			
			if(piRender->AreTexturesEnabled())
			{
				for(unsigned int x=0;x<pBuffer->vTextureLevels.size();x++)
				{
					SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[x];
					if(pTextureLevel && pTextureLevel->texture.m_piTexture && pTextureLevel->pTexVertexArray)
					{
						glClientActiveTextureARB(GL_TEXTURE0_ARB+x);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glTexCoordPointer(2,GL_FLOAT,0,pBuffer->vTextureLevels[x]->pTexVertexArray);
					}
				}
			}
		}
	}
}

void COpenGLModel::CallRenderBuffer(IGenericRender *piRender, unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer )
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation,nFrame,nBuffer);
	if(pBuffer==NULL){return;}

	if(pBuffer->nBufferObject)
	{
		glDrawElements(GL_TRIANGLES,pBuffer->nFaces*3,GL_UNSIGNED_INT,NULL);
	}
	else
	{
		glDrawElements(GL_TRIANGLES,pBuffer->nFaces*3,GL_UNSIGNED_INT,pBuffer->pFaceVertexIndexes);
	}
}

void COpenGLModel::UnPrepareRenderBuffer(IGenericRender *piRender, unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer ,bool bRenderingShadow)
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation,nFrame,nBuffer);
	if(pBuffer==NULL){return;}

	if(pBuffer->nBufferObject){glBindBuffer(GL_ARRAY_BUFFER,0);}
	if(pBuffer->nIndexesBufferObject){glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);}

	if(pBuffer->pVertexArray){glDisableClientState(GL_VERTEX_ARRAY);}
	if(!bRenderingShadow && pBuffer->pNormalArray){glDisableClientState(GL_NORMAL_ARRAY);}
	if(!bRenderingShadow && pBuffer->pColorArray)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		glDisable(GL_COLOR_MATERIAL);
	}
	if(!bRenderingShadow && piRender->AreTexturesEnabled())
	{
		for(unsigned int x=0;x<pBuffer->vTextureLevels.size();x++)
		{
			if(pBuffer->vTextureLevels[x]->texture.m_piTexture && pBuffer->vTextureLevels[x]->pTexVertexArray)
			{
				glClientActiveTextureARB(GL_TEXTURE0_ARB+x);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				piRender->UnselectTexture(x);
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
		//else{RTTRACE("Trace hit");}
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
	fShininess=0;
	fOpacity=1.0;

	nBufferObject=0;
	nIndexesBufferObject=0;

	nVertexes=0;
	nFaces=0;
	pVertexArray=NULL;
	pNormalArray=NULL;
	pColorArray=NULL;
	pFaceVertexIndexes=NULL;
}

SModelRenderBuffer::~SModelRenderBuffer()
{
	for(unsigned x=0;x<vTextureLevels.size();x++){delete vTextureLevels[x];}
	vTextureLevels.clear();

	if(nBufferObject!=0){glDeleteBuffers(1,&nBufferObject);nBufferObject=0;}
	if(nIndexesBufferObject!=0){glDeleteBuffers(1,&nIndexesBufferObject);nIndexesBufferObject=0;}

	delete [] pVertexArray;
	delete [] pNormalArray;
	delete [] pColorArray;
	delete [] pFaceVertexIndexes;

	pVertexArray=NULL;
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

