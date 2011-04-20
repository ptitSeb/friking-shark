#include "./stdafx.h"
#include "OpenGLModel.h"

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
	COpenGLModelVertexKey(){memset(t,0,sizeof(t));i=0;}
};

bool COpenGLModel::LoadFromFile()
{
	RemoveAnimations();
	
	int nStartTime=GetTimeStamp();
	
	CASEFileType	file;
	
	std::string sFileName=m_sFileName;
	if(!file.Open(sFileName.c_str()))
	{
	  	char pPath[MAX_PATH];
		GetFileFolder(sFileName.c_str(),pPath);

		char sExt[MAX_PATH]={0};
		GetExtension(sFileName.c_str(),sExt);
		if(pPath[0]==0 || strcmp(pPath,".")==0)
		{
			std::string sTemp="Models/";
			sTemp+=sFileName;
			sFileName=sTemp;
		}
		
		if(!file.Open(sFileName.c_str()))
		{
			RTTRACE("COpenGLModel::LoadFromFile -> Failed to load model %s",m_sFileName.c_str());
			return false;
		}
	}
	SModelAnimation *pAnimation=new SModelAnimation;
	m_vAnimations.push_back(pAnimation);
	
	CGenericTextureWrapper *pTextures=new CGenericTextureWrapper [file.m_vMaterials.size()];
	
	for (int f=0;f<file.m_Scene.nFrameCount;f++)
	{
		SModelFrame *pFrame=new SModelFrame;
		pAnimation->vFrames.push_back(pFrame);

		for (int m=-1;m<(int)file.m_vMaterials.size();m++)
		{
		    // Preparacion del material
			S3DSMaterial *p3DSMaterial=(m>=0)?file.m_vMaterials[m]:NULL;
			
			CMatrix ms,mr;
			SModelRenderBuffer *pBuffer=new SModelRenderBuffer;
			if(p3DSMaterial)
			{
				pBuffer->vAmbientColor=p3DSMaterial->vAmbientColor;
				pBuffer->vDiffuseColor=p3DSMaterial->vDiffuseColor;
				pBuffer->vSpecularColor=p3DSMaterial->vSpecularColor;
				// From http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=172040
				//			Shininess = exp (MATERIAL_SHINE * 100.0 / 8.0 * log (2.0));
				//			EffectiveSpecularColor = MATERIAL_SHINESTRENGTH * MATERIAL_SPECULAR + (1 - MATERIAL_SHINESTRENGTH) * MATERIAL_DIFFUSE;
				pBuffer->vSpecularColor=p3DSMaterial->vSpecularColor;
				pBuffer->fShininess=p3DSMaterial->fShininess*64.0;
				pBuffer->fOpacity=(float)(1.0-p3DSMaterial->fTranparency);

				// Calculo de las matrices de transformacion de las coordenadas de textura
				CVector vTextureCenter(0.5,0.5,0);
				ms.S(p3DSMaterial->fTextureUScale,p3DSMaterial->fTextureVScale,1,vTextureCenter);
				mr.R(CVector(0,0,1),p3DSMaterial->fTextureAngle,vTextureCenter);
			}
			else
			{
				pBuffer->vAmbientColor=CVector(0.5,0.5,0.5);
				pBuffer->vDiffuseColor=CVector(0.5,0.5,0.5);
				pBuffer->vSpecularColor=CVector(0.5,0.5,0.5);
				pBuffer->fShininess=1.0;
				pBuffer->fOpacity=1.0;
			}
			
		    // Contabilizacion de las caras de todos los objetos para este material 
			int nMaterialFaces=0;
			for (unsigned int o=0;o<file.m_vObjects.size();o++)
			{
				S3DSObject *pObject=file.m_vObjects[o];
				if (pObject->bVisible && p3DSMaterial && pObject->dwMaterialId==p3DSMaterial->dwMaterialId)
				{
					if(p3DSMaterial->bSubMaterial)
					{
						S3DSFrame  *p3DSFrame=file.m_vObjects[o]->vAnimationFrames[f];
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
		    
			pBuffer->pFaceVertexIndexes=new GLuint[nMaterialFaces*3];
			pBuffer->pVertexArray=new GLfloat[nMaterialFaces*3*3];
			pBuffer->pNormalArray=new GLfloat[nMaterialFaces*3*3];
			pBuffer->pColorArray=new GLfloat[nMaterialFaces*3*4];
			
			GLuint  *pFaceVertexCursor=pBuffer->pFaceVertexIndexes;
			GLfloat *pVertexCursor=pBuffer->pVertexArray;
			GLfloat *pNormalCursor=pBuffer->pNormalArray;
			GLfloat *pColorCursor=pBuffer->pColorArray;
			GLfloat *pTextCursor=NULL;
			SModelTextureLevel *pTextureLevel=NULL;
			
			if (p3DSMaterial && p3DSMaterial->sFile[0]!=0)
			{
				pTextureLevel=new SModelTextureLevel;
				pTextureLevel->pTexVertexArray=new GLfloat[nMaterialFaces*3*2];
				pTextCursor=pTextureLevel->pTexVertexArray;
				pBuffer->vTextureLevels.push_back(pTextureLevel);
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
			

			std::map<COpenGLModelVertexKey,unsigned int> mVertexes;
			std::map<COpenGLModelVertexKey,unsigned int>::iterator i;
			
			for (unsigned int o=0;o<file.m_vObjects.size();o++)
			{
				S3DSObject *pObject=file.m_vObjects[o];
				if (!pObject->bVisible || p3DSMaterial==NULL || pObject->dwMaterialId!=p3DSMaterial->dwMaterialId) {continue;}

//				S3DSMaterial		*p3DSMaterial=(pObject->dwMaterialId!=(unsigned int)-1)?file.m_vMaterials[pObject->dwMaterialId]:NULL;
				S3DSFrame  		   	*p3DSFrame=file.m_vObjects[o]->vAnimationFrames[f];

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

				float ambient[]={(float)pBuffer->vDiffuseColor.c[0],(float)pBuffer->vDiffuseColor.c[1],(float)pBuffer->vDiffuseColor.c[2]};
				float opacity=pBuffer->fOpacity;	

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

						COpenGLModelVertexKey key;
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

			pBuffer->nVertexes=mVertexes.size();
			pBuffer->nFaces=nMaterialFaces;

			GLfloat *pVertexArray=new GLfloat[pBuffer->nVertexes*3];
			GLfloat *pNormalArray=new GLfloat[pBuffer->nVertexes*3];
			GLfloat *pColorArray=new GLfloat[pBuffer->nVertexes*4];
			
			memcpy(pVertexArray,pBuffer->pVertexArray,sizeof(GLfloat)*pBuffer->nVertexes*3);
			memcpy(pNormalArray,pBuffer->pNormalArray,sizeof(GLfloat)*pBuffer->nVertexes*3);
			memcpy(pColorArray,pBuffer->pColorArray,sizeof(GLfloat)*pBuffer->nVertexes*4);
			
			delete [] pBuffer->pVertexArray;
			delete [] pBuffer->pNormalArray;
			delete [] pBuffer->pColorArray;
			  
			pBuffer->pVertexArray=pVertexArray;
			pBuffer->pNormalArray=pNormalArray;
			pBuffer->pColorArray=pColorArray;
			
			// Las coordenadas de textura son opcionales.
			if(pTextureLevel)
			{
			  GLfloat *pTextArray=new GLfloat[pBuffer->nVertexes*2];
			  memcpy(pTextArray,pTextureLevel->pTexVertexArray,sizeof(GLfloat)*pBuffer->nVertexes*2);
			  delete [] pTextureLevel->pTexVertexArray;
			  pTextureLevel->pTexVertexArray=pTextArray;
			}

			// Si hay vertices para el material se añade el render buffer 
			// y se carga la textura si hay alguna asignada
			if (pBuffer->nVertexes)
			{
				if (pBuffer->vTextureLevels.size())
				{
					// Las texturas se cargan a partir de un array global para cargarlas
					// una sola vez ya que son comunes a todos lo frames del modelo
					// Despues se asignan con Attach a los render buffers que los usen
					if (pTextures[m].m_piTexture==NULL)
					{
						pTextures[m].Create(m_piSystem,"Texture","");
						if (pTextures[m].m_piTexture)
						{
							pTextures[m].m_piTexture->Load(p3DSMaterial->sFile,NULL,NULL,pBuffer->fOpacity);
						}
					}
					if (pTextures[m].m_piTexture)
					{
						pBuffer->vTextureLevels[0]->texture.Attach(pTextures[m].m_piTexture);
					}
				}
				pFrame->vRenderBuffers.push_back(pBuffer);
			}
			else
			{
				delete pBuffer;
			}
		}
	}
	
	delete [] pTextures;
	pTextures=NULL;
	
	LoadBSP(sFileName.c_str());
	UpdateFrameBuffers();

	RTTRACE("COpenGLModel::LoadFromFile -> Loaded model %s (%d ms)",m_sFileName.c_str(),GetTimeStamp()-nStartTime);
	return true;
}

void COpenGLModel::GetGeometry(std::vector<CPolygon*> *pPolygons)
{
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
	strcpy(sGBSFile,pFileName);
	ReplaceExtension(sGBSFile,".gbs");

	bool bGenerateBSP=false;
	std::vector<CPolygon*> vGeometry;
	std::vector<CPolygon*> vBSPGeometry;

	CGBSFileType gbsFile;
	if(!gbsFile.Load(sGBSFile,&pModelBSP,m_bAutoUpdateBSP?&vBSPGeometry:NULL))
	{
		bGenerateBSP=m_bAutoGenerateBSP || m_bAutoUpdateBSP;
		if(bGenerateBSP){GetGeometry(&vGeometry);}
	}
	else
	{
		if(m_bAutoUpdateBSP)
		{
			GetGeometry(&vGeometry);
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
	return (m_pModelBSP!=NULL);
}

bool COpenGLModel::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bResult=CSystemObjectBase::Unserialize(piNode);
	if(bResult){bResult=LoadFromFile();}
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


void COpenGLModel::GetRenderBufferTextureMatrix(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,CMatrix *pMatrix)
{
	SModelRenderBuffer *pBuffer=GetRenderBuffer(nAnimation, nFrame, nBuffer);
	if(pBuffer==NULL){return;}
	while(nTextureLevel>=pBuffer->vTextureLevels.size()){pBuffer->vTextureLevels.push_back(new SModelTextureLevel);}
	SModelTextureLevel *pTextureLevel= pBuffer->vTextureLevels[nTextureLevel];
	*pMatrix=pTextureLevel->texMatrix;
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