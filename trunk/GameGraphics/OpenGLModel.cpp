#include "StdAfx.h"
#include ".\openglmodel.h"

COpenGLModel::COpenGLModel(void)
{
	m_bLoadBSP=false;
	m_bAutoGenerateBSP=false;
	m_bAutoUpdateBSP=false;
	m_pModelBSP=NULL;
}

COpenGLModel::~COpenGLModel(void)
{
	RemoveAnimations();
}

std::string	 COpenGLModel::GetFileName(){return m_sFileName;}


class COpenGLModelVertexKey
{
public:
	double c[3];double t[2];double n[3];double col[3];unsigned int i;
	bool operator<(const COpenGLModelVertexKey &v2) const{if(c[0]>v2.c[0]){return false;}if(c[0]<v2.c[0]){return true;}if(c[1]>v2.c[1]){return false;}if(c[1]<v2.c[1]){return true;}if(c[2]>v2.c[2]){return false;}if(c[2]<v2.c[2]){return true;}if(t[0]>v2.t[0]){return false;}if(t[0]<v2.t[0]){return true;}if(t[1]>v2.t[1]){return false;}if(t[1]<v2.t[1]){return true;}if(n[0]>v2.n[0]){return false;}if(n[0]<v2.n[0]){return true;}if(n[1]>v2.n[1]){return false;}if(n[1]<v2.n[1]){return true;}if(n[2]>v2.n[2]){return false;}if(n[2]<v2.n[2]){return true;}return false;}
	COpenGLModelVertexKey(CVector vc,CVector vt,CVector vn,CVector vcol,int vi){int x;for(x=0;x<3;x++){c[x]=vc.c[x];}for(x=0;x<2;x++){t[x]=vt.c[x];}for(x=0;x<3;x++){n[x]=vn.c[x];}for(x=0;x<3;x++){col[x]=vcol.c[x];}i=vi;}
	COpenGLModelVertexKey(){memset(c,0,sizeof(c));memset(t,0,sizeof(t));memset(n,0,sizeof(n));memset(col,0,sizeof(col));i=0;}
};

bool COpenGLModel::LoadFromFile()
{
	RemoveAnimations();

	RTTRACE("COpenGLModel::LoadFromFile -> Loading model %s",m_sFileName.c_str());


	CResourceStore *pStore=new CResourceStore;
	CModel *pModel=pStore->LoadModel(m_sFileName);
	if(!pModel)
	{
		RTTRACE("COpenGLModel::LoadFromFile -> Failed to load model %s",m_sFileName.c_str());
	}

	for(int x=0;pModel && x<pModel->m_nAnimations;x++)
	{
		SModelAnimation *pAnimation=new SModelAnimation;
		m_vAnimations.push_back(pAnimation);

		for(int y=0;y<pModel->m_pAnimations[x].m_nFrames;y++)
		{
			CFrame *pModelFrame=&pModel->m_pAnimations[x].m_pFrames[y];
			SModelFrame *pFrame=new SModelFrame;
			pAnimation->vFrames.push_back(pFrame);

			set<CMaterial *> mMaterials;
			set<CMaterial *>::iterator i;
			int x,m;
			unsigned int v;
			for(x=0;x<pModelFrame->m_nPolygons;x++)
			{
				if(pModelFrame->m_pPolygons[x].m_pMaterial){mMaterials.insert(pModelFrame->m_pPolygons[x].m_pMaterial);}
			}

			for(i=mMaterials.begin(),m=0;i!=mMaterials.end();i++,m++)
			{
				map<COpenGLModelVertexKey,DWORD> mVertexes;
				map<COpenGLModelVertexKey,DWORD>::iterator iVertexes;

				SModelRenderBuffer *pBuffer=new SModelRenderBuffer;
				pFrame->vRenderBuffers.push_back(pBuffer);

				CMaterial					*pMaterial=*i;

				for(x=0;x<pModelFrame->m_nPolygons;x++)
				{
					if(pModelFrame->m_pPolygons[x].m_pMaterial==pMaterial)
					{
						pBuffer->nFaces++;

						CMaterialPolygon *pPolygon=&pModelFrame->m_pPolygons[x];
						for(v=0;v<pPolygon->m_nVertexes;v++)
						{
							COpenGLModelVertexKey key(pPolygon->m_pVertexes[v],pPolygon->m_pTextureCoords[v],pPolygon->m_pVertexNormals[v],pPolygon->m_pVertexColors?pPolygon->m_pVertexColors[v]:RGBToVector(pMaterial->cAmbientColor),mVertexes.size());
							if(mVertexes.find(key)==mVertexes.end())
							{
								mVertexes[key]=mVertexes.size();
							}
						}
					}
				}

				pBuffer->nVertexes=mVertexes.size();
				pBuffer->pVertexArray=new GLfloat[pBuffer->nVertexes*3];
				pBuffer->pNormalArray=new GLfloat[pBuffer->nVertexes*3];
				pBuffer->pColorArray=new GLfloat[pBuffer->nVertexes*4];
				pBuffer->pFaceVertexIndexes=new unsigned int[pBuffer->nFaces*3];
				pBuffer->fOpacity=pMaterial->fOpacity;
				pBuffer->fShininess=pMaterial->fShininess;
				pBuffer->vAmbientColor=RGBToVector(pMaterial->cAmbientColor);
				pBuffer->vDiffuseColor=RGBToVector(pMaterial->cDiffuseColor);
				pBuffer->vSpecularColor=RGBToVector(pMaterial->cSpecularColor);

				SModelTextureLevel *pTextureLevel=NULL;
				if(pMaterial->nTextureIndex!=-1)
				{
					pTextureLevel=new SModelTextureLevel;
					pTextureLevel->pTexVertexArray=new GLfloat[pBuffer->nVertexes*2];
					pTextureLevel->texture.Create(m_piSystem,"Texture","");
					if(pTextureLevel->texture.m_piTexture)
					{
						pTextureLevel->texture.m_piTexture->Load(pMaterial->pTexture->m_sName,NULL,NULL,pBuffer->fOpacity);
					}
					pBuffer->vTextureLevels.push_back(pTextureLevel);
				}

				int nCurVert=0;
				int nCurTex=0;
				int nCurCol=0;
				for(iVertexes=mVertexes.begin();iVertexes!=mVertexes.end();iVertexes++)
				{
					const COpenGLModelVertexKey *pVertex=&iVertexes->first;

					pBuffer->pVertexArray[pVertex->i*3]=(float)pVertex->c[0];
					pBuffer->pVertexArray[pVertex->i*3+1]=(float)pVertex->c[1];
					pBuffer->pVertexArray[pVertex->i*3+2]=(float)pVertex->c[2];

					pBuffer->pNormalArray[pVertex->i*3]=(float)pVertex->n[0];
					pBuffer->pNormalArray[pVertex->i*3+1]=(float)pVertex->n[1];
					pBuffer->pNormalArray[pVertex->i*3+2]=(float)pVertex->n[2];

					pBuffer->pColorArray[pVertex->i*4]=(float)pVertex->col[0];
					pBuffer->pColorArray[pVertex->i*4+1]=(float)pVertex->col[1];
					pBuffer->pColorArray[pVertex->i*4+2]=(float)pVertex->col[2];
					pBuffer->pColorArray[pVertex->i*4+3]=(float)pMaterial->fOpacity;

					if(pTextureLevel)
					{
						pTextureLevel->pTexVertexArray[pVertex->i*2]=(float)pVertex->t[0];
						pTextureLevel->pTexVertexArray[pVertex->i*2+1]=(float)pVertex->t[1];
					}

					nCurVert+=3;
					nCurTex+=2;
					nCurCol+=4;
				}

				int nIndex=0;
				for(x=0;x<pModelFrame->m_nPolygons;x++)
				{
					if(pModelFrame->m_pPolygons[x].m_pMaterial==pMaterial)
					{
						CMaterialPolygon *pPolygon=&pModelFrame->m_pPolygons[x];
						for(v=0;v<pPolygon->m_nVertexes;v++)
						{
							COpenGLModelVertexKey key(pPolygon->m_pVertexes[v],pPolygon->m_pTextureCoords[v],pPolygon->m_pVertexNormals[v],Origin,0);
							pBuffer->pFaceVertexIndexes[nIndex+v]=mVertexes[key];
						}
						nIndex+=3;
					}
				}
			}

		}
	}
	CFrame *pFrame=pModel?pModel->GetFrame(0,0):NULL;
	if(pFrame)
	{	
		CBSPNode *pModelBSP=NULL;;
		char sGBSFile[MAX_PATH]={0},sDrive[MAX_PATH]={0},sPath[MAX_PATH]={0},sFileName[MAX_PATH]={0},sExt[MAX_PATH]={0},sOldDirectory[MAX_PATH]={0};
		_splitpath(m_sFileName.c_str(),sDrive,sPath,sFileName,sExt);
		sprintf(sGBSFile,"%s%s%s.gbs",sDrive,sPath,sFileName);

		bool bGenerateBSP=false;
		vector<CPolygon*> vGeometry;
		vector<CPolygon*> vBSPGeometry;
		CGBSFileType gbsFile;
		if(!gbsFile.Load(sGBSFile,&pModelBSP,m_bAutoUpdateBSP?&vBSPGeometry:NULL))
		{
			bGenerateBSP=m_bAutoGenerateBSP || m_bAutoUpdateBSP;
			if(bGenerateBSP){pFrame->GetGeometry(&vGeometry);}
		}
		else
		{
			if(m_bAutoUpdateBSP)
			{
				pFrame->GetGeometry(&vGeometry);
				if(!CGBSFileType::CompareGeometricData(&vGeometry,&vBSPGeometry))
				{
					bGenerateBSP=true;				
				}
			}
		}

		if(bGenerateBSP)
		{
			pModelBSP=BSPFromPolygonVector(NULL,1,&vGeometry,CONTENT_NODE,NULL,true);
			if(pModelBSP)
			{
				gbsFile.Save(sGBSFile,pModelBSP,&vGeometry);
			}
		}
		for(unsigned int x=0;x<vGeometry.size();x++){delete vGeometry[x];}
		for(unsigned int x=0;x<vBSPGeometry.size();x++){delete vBSPGeometry[x];}

		vGeometry.clear();
		vBSPGeometry.clear();

		m_pModelBSP=pModelBSP;
	
	}

	delete pStore;
	UpdateFrameBuffers();

	RTTRACE("COpenGLModel::LoadFromFile -> Finished loading model %s",m_sFileName.c_str());

	return pModel!=NULL;
}


bool COpenGLModel::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	if(bResult){bResult=LoadFromFile();}
	return bResult;
}


bool COpenGLModel::Load( string sFileName )
{
	m_sFileName=sFileName;
	return LoadFromFile();
}

void COpenGLModel::Create()
{
	m_bLoadBSP=false;
	m_bAutoGenerateBSP=false;
	m_bAutoUpdateBSP=false;
	RemoveAnimations();
}



void COpenGLModel::Render(IGenericRender *piRender,unsigned long nAnimation,unsigned long nFrame)
{
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

					DWORD dwOffset=0;
					if(pBuffer->pVertexArray)
					{
						memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pVertexArray,pBuffer->nVertexes*3*sizeof(GLfloat));
						dwOffset+=pBuffer->nVertexes*3*sizeof(GLfloat);
					}
					if(pBuffer->pNormalArray)
					{
						memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pNormalArray,pBuffer->nVertexes*3*sizeof(GLfloat));
						dwOffset+=pBuffer->nVertexes*3*sizeof(GLfloat);
					}
					if(pBuffer->pColorArray)
					{
						memcpy(((BYTE*)pBufferObject)+dwOffset,pBuffer->pColorArray,pBuffer->nVertexes*4*sizeof(GLfloat));
						dwOffset+=pBuffer->nVertexes*4*sizeof(GLfloat);
					}
					for(unsigned long nTexture=0;nTexture<pBuffer->vTextureLevels.size();nTexture++)
					{
						SModelTextureLevel *pTextureLevel=pBuffer->vTextureLevels[nTexture];
						if(pTextureLevel/* && pTextureLevel->texture.m_piTexture*/ && pTextureLevel->pTexVertexArray)
						{
							memcpy(((BYTE*)pBufferObject)+dwOffset,pTextureLevel->pTexVertexArray,pBuffer->nVertexes*2*sizeof(GLfloat));
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
	return m_vAnimations.size();
}

unsigned long COpenGLModel::GetAnimationFrames(unsigned long nAnimation)
{
	if(nAnimation>=m_vAnimations.size()){return 0;}
	return m_vAnimations[nAnimation]->vFrames.size();
}	

unsigned long COpenGLModel::GetFrameRenderBuffers( unsigned long nAnimation,unsigned long nFrame )
{
	if(nAnimation>=m_vAnimations.size()){return 0;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return 0;}
	return m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.size();
}

void COpenGLModel::GetFrameBBox(unsigned long nAnimation,unsigned long nFrame,CVector *pvMins,CVector *pvMaxs)
{
	if(nAnimation>=m_vAnimations.size()){return;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return;}
	*pvMins=m_vAnimations[nAnimation]->vFrames[nFrame]->vMins;
	*pvMaxs=m_vAnimations[nAnimation]->vFrames[nFrame]->vMaxs;
}

CVector COpenGLModel::GetFrameSize( unsigned long nAnimation,unsigned long nFrame )
{
	if(nAnimation>=m_vAnimations.size()){return Origin;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return Origin;}
	return m_vAnimations[nAnimation]->vFrames[nFrame]->vSize;
}

double COpenGLModel::GetFrameRadius( unsigned long nAnimation,unsigned long nFrame )
{
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
	if(nAnimation>=m_vAnimations.size()){return 0;}
	if(nFrame>=m_vAnimations[nAnimation]->vFrames.size()){return 0;}
	SModelRenderBuffer *pRenderBuffer=new SModelRenderBuffer;
	m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.push_back(pRenderBuffer);
	return (m_vAnimations[nAnimation]->vFrames[nFrame]->vRenderBuffers.size()-1);
}

SModelRenderBuffer *COpenGLModel::GetRenderBuffer( unsigned long nAnimation, unsigned long nFrame, unsigned long nBuffer )
{
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
	if(ppiTexture){*ppiTexture=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(nTextureLevel>=pBuffer->vTextureLevels.size()){return;}
	if(ppiTexture){*ppiTexture=ADD(pBuffer->vTextureLevels[nTextureLevel]->texture.m_piTexture);}
}

void COpenGLModel::GetRenderBufferVertexes( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes )
{
	if(pVertexes){*pVertexes=0;}
	if(ppVertexes){*ppVertexes=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(pVertexes){*pVertexes=pBuffer->nVertexes;}
	if(ppVertexes){*ppVertexes=pBuffer->pVertexArray;}
}

void COpenGLModel::GetRenderBufferFaces( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes )
{
	if(pFaces){*pFaces=0;}
	if(ppFacesVertexes){*ppFacesVertexes=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(pFaces){*pFaces=pBuffer->nFaces;}
	if(ppFacesVertexes){*ppFacesVertexes=pBuffer->pFaceVertexIndexes;}
}

void COpenGLModel::GetRenderBufferNormals( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormals )
{
	if(ppNormals){*ppNormals=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppNormals){*ppNormals=pBuffer->pNormalArray;}
}

void COpenGLModel::GetRenderBufferColors( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppColors )
{
	if(ppColors){*ppColors=NULL;}
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	if(ppColors){*ppColors=pBuffer->pColorArray;}
}

void COpenGLModel::GetRenderBufferTextureCoords( unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes )
{
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
	return m_pModelBSP;
}

void COpenGLModel::SetBSPOptions(bool bLoad,bool bAutoGenerate,bool bAutoUpdate)
{
	m_bLoadBSP=bLoad;
	m_bAutoGenerateBSP=bAutoGenerate;
	m_bAutoUpdateBSP=bAutoUpdate;
}

void COpenGLModel::GetBSPOptions(bool *pbLoad,bool *pbAutoGenerate,bool *pbAutoUpdate)
{
	if(pbLoad){*pbLoad=m_bLoadBSP;}
	if(pbAutoGenerate){*pbAutoGenerate=m_bAutoGenerateBSP;}
	if(pbAutoUpdate){*pbAutoUpdate=m_bAutoUpdateBSP;}
}

void COpenGLModel::PrepareRenderBuffer(IGenericRender *piRender, unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer ,bool bRenderingShadow)
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation,nFrame,nBuffer);
	if(pBuffer==NULL){return;}

	if(!bRenderingShadow && piRender->AreTexturesEnabled())
	{
		for(unsigned int x=0;x<pBuffer->vTextureLevels.size();x++)
		{
			if(pBuffer->vTextureLevels[x]->texture.m_piTexture && pBuffer->vTextureLevels[x]->pTexVertexArray)
			{
				piRender->SelectTexture(pBuffer->vTextureLevels[x]->texture.m_piTexture,x);
			}
		}
	}
	int tex=0,vert=0;
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
		if(!bRenderingShadow && pBuffer->pNormalArray)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT,0,(void*)(pBuffer->nVertexes*sizeof(GLfloat)*nOffset));
			nOffset+=3;
		}
		if(!bRenderingShadow && pBuffer->pColorArray)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4,GL_FLOAT,0,(void*)(pBuffer->nVertexes*sizeof(GLfloat)*nOffset));
			nOffset+=4;
		}
		if(!bRenderingShadow && piRender->AreTexturesEnabled())
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
	else
	{
		if(pBuffer->pVertexArray)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT,0,pBuffer->pVertexArray);
		}
		if(!bRenderingShadow && pBuffer->pNormalArray)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT,0,pBuffer->pNormalArray);
		}
		if(!bRenderingShadow && pBuffer->pColorArray)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4,GL_FLOAT,0,pBuffer->pColorArray);
		}

		if(!bRenderingShadow && piRender->AreTexturesEnabled())
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
	if(!bRenderingShadow && pBuffer->pColorArray){glDisableClientState(GL_COLOR_ARRAY);}
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
		else
		{
			RTTRACE("Trace hit");
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

SModelTextureLevel::SModelTextureLevel()
{
	pTexVertexArray=NULL;
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