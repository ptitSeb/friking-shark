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


#pragma once

struct SModelTextureLevel
{
	CGenericTextureWrapper   texture;
	GLfloat					*pTexVertexArray;
	CMatrix 				 texMatrix;
	bool					 bTextMatrixIdentity;
	

	 SModelTextureLevel();
	~SModelTextureLevel();
};

struct SModelRenderBuffer
{
	CVector		vAmbientColor;
	CVector		vDiffuseColor;
	CVector		vSpecularColor;
	float		fShininess;
	float		fOpacity;

	bool        bVertexArrayConfigured;
	GLuint		nVertexArrayObject;
	GLuint		nBufferObject;
	GLuint		nIndexesBufferObject;

	int			nVertexes;
	int			nFaces;
	GLfloat		*pVertexArray;
	GLfloat		*pNormalArray;
	GLfloat		*pNormalMapArray;
	GLfloat		*pTangentArray;
	GLfloat		*pBitangentArray;
	GLfloat		*pColorArray;
	GLuint 		*pFaceVertexIndexes;

	int 		nVertexStride;
	int 		nColorStride;
	int 		nNormalStride;
	int 		pTexStride[2];
	int 		nNormalMapStride;
	int 		nTangentStride;
	int 		nBitangentStride;

	int 		nVertexOffset;
	int 		nColorOffset;
	int 		nNormalOffset;
	int 		pTexOffset[2];
	int 		nNormalMapOffset;
	int 		nTangentOffset;
	int 		nBitangentOffset;
	
	CGenericTextureWrapper   normalMap;
	
	std::vector<SModelTextureLevel *> vTextureLevels;

	SModelRenderBuffer();
	~SModelRenderBuffer();
};


struct SModelFrame
{
	CVector vMaxs;
	CVector vMins;
	CVector vSize;
	double  dRadius;

	std::vector<SModelRenderBuffer *> vRenderBuffers;

	SModelFrame();
	~SModelFrame();
};

struct SModelAnimation
{
	std::vector<SModelFrame *> vFrames;

	SModelAnimation();
	~SModelAnimation();
};

class COpenGLModel : virtual public CSystemObjectBase,virtual public IGenericModel,virtual public IOpenGLModel
{
	std::string					 	m_sFileName;
	std::vector<SModelAnimation *>  m_vAnimations;
	CBSPNode						*m_pModelBSP;
	bool						 	m_bLoadBSP;
	bool							m_bLoadPending;
	bool							m_bLoadResult;

	bool LoadFromFile();
	bool LoadBSP(const char *pFileName);
	bool Unserialize(ISystemPersistencyNode *piNode);
	
	void GetGeometry(std::vector<CPolygon*> *pPolygons);

	SModelRenderBuffer *GetRenderBuffer( unsigned long nAnimation, unsigned long nFrame, unsigned long nBuffer );

	void UpdateTangentBasis(SModelRenderBuffer *pBuffer);
	void ClearTangentBasis(SModelRenderBuffer *pBuffer);
	
	void BuildStridedBuffer(SModelRenderBuffer *pBuffer);

public:

	// IGenericModel
	virtual bool Load(std::string sFileName);
	void		 Create();

	std::string	 GetFileName();

	unsigned long GetAnimations();
	unsigned long GetAnimationFrames(unsigned long nAnimation);
	unsigned long GetFrameRenderBuffers(unsigned long nAnimation,unsigned long nFrame);
	CVector		  GetFrameSize(unsigned long nAnimation,unsigned long nFrame);
	double		  GetFrameRadius(unsigned long nAnimation,unsigned long nFrame);
	void		  GetFrameBBox(unsigned long nAnimation,unsigned long nFrame,CVector *pvMins,CVector *pvMaxs);

	unsigned long AddAnimation();
	unsigned long AddAnimationFrame(unsigned long nAnimation);
	unsigned long AddRenderBuffer(unsigned long nAnimation,unsigned long nFrame);

	void		  RemoveRenderBuffers(unsigned long nAnimation,unsigned long nFrame);
	void		  RemoveFrames(unsigned long nAnimation);
	void		  RemoveAnimations();

	void		  SetRenderBufferMaterial(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector vAmbientColor,CVector vDiffuseColor,CVector vSpecularColor, float fShininess, float fOpacity);
	void		  SetRenderBufferTexture(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture *piTexture);
	void		  SetRenderBufferTextureMatrix(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,CMatrix *pMatrix);
	void		  SetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nVertexes,float *pVertexes);
	void		  SetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes);
	void		  SetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormals);
	void		  SetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pColors);
	void		  SetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float *pTexVertexes);
	void		  SetRenderBufferNormalMap(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,IGenericTexture *piTexture);
	void		  SetRenderBufferNormalMapCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormalMapVertexes);
	
	void		  GetRenderBufferMaterial(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor, float *pfShininess, float *pfOpacity);
	void		  GetRenderBufferTexture(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture **ppiTexture);
	void		  GetRenderBufferTextureMatrix(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,CMatrix *pMatrix);
	void		  GetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes);
	void		  GetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes);
	void		  GetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormals);
	void		  GetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppColors);
	void		  GetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes);
	void		  GetRenderBufferNormalMap(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,IGenericTexture **ppiTexture);
	void		  GetRenderBufferNormalMapCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormalMapVertexes);
	
	void		  Render(IGenericRender *piRender,unsigned long nAnimation,unsigned long nFrame);

	CBSPNode	 *GetBSP();
	void		  GetBSPOptions(bool *pbLoad);
	void		  SetBSPOptions(bool bLoad);
	CTraceInfo	  GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

	void UpdateBufferObjects();
	
	bool Prepare();


	// IOpenGLModel

	void PrepareRenderBuffer(IOpenGLRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings);
	void CallRenderBuffer(IOpenGLRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,unsigned int nInstances);
	void UnPrepareRenderBuffer(IOpenGLRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow,SOpenGLRenderMappings *pRenderMappings);
	
	BEGIN_PROP_MAP(COpenGLModel)
		PROP_FLAGS(m_sFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bLoadBSP,"LoadBSP",false,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	COpenGLModel(void);
	~COpenGLModel(void);
};
