#pragma once

struct SModelTextureLevel
{
	CGenericTextureWrapper texture;
	GLfloat			*pTexVertexArray;

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

	GLuint		nBufferObject;
	GLuint		nIndexesBufferObject;

	int			nVertexes;
	int			nFaces;
	GLfloat		*pVertexArray;
	GLfloat		*pNormalArray;
	GLfloat		*pColorArray;
	GLuint 		*pFaceVertexIndexes;

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
	bool						 	m_bAutoGenerateBSP;
	bool						 	m_bAutoUpdateBSP;

	bool LoadFromFile();
	bool LoadBSP(const char *pFileName);
	bool Unserialize(ISystemPersistencyNode *piNode);
	
	void GetGeometry(std::vector<CPolygon*> *pPolygons);

	SModelRenderBuffer *GetRenderBuffer( unsigned long nAnimation, unsigned long nFrame, unsigned long nBuffer );

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
	void		  SetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nVertexes,float *pVertexes);
	void		  SetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nFaces,unsigned int *pFacesVertexes);
	void		  SetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pNormals);
	void		  SetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float *pColors);
	void		  SetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float *pTexVertexes);

	void		  GetRenderBufferMaterial(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,CVector *pvAmbientColor,CVector *pvDiffuseColor,CVector *pvSpecularColor, float *pfShininess, float *pfOpacity);
	void		  GetRenderBufferTexture(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,IGenericTexture **ppiTexture);
	void		  GetRenderBufferVertexes(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pVertexes,float **ppVertexes);
	void		  GetRenderBufferFaces(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long *pFaces,unsigned int **ppFacesVertexes);
	void		  GetRenderBufferNormals(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppNormals);
	void		  GetRenderBufferColors(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,float **ppColors);
	void		  GetRenderBufferTextureCoords(unsigned long nAnimation,unsigned long nFrame,unsigned long nBuffer,unsigned long nTextureLevel,float **ppTexVertexes);

	void		  Render(IGenericRender *piRender,unsigned long nAnimation,unsigned long nFrame);

	CBSPNode	 *GetBSP();
	void		  GetBSPOptions(bool *pbLoad,bool *pbAutoGenerate,bool *pbAutoUpdate);
	void		  SetBSPOptions(bool bLoad,bool bAutoGenerate,bool bAutoUpdate);
	CTraceInfo	  GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

	void UpdateFrameBuffers();


	// IOpenGLModel

	void PrepareRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow);
	void CallRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer);
	void UnPrepareRenderBuffer(IGenericRender *piRender,unsigned int nAnimation,unsigned int nFrame, unsigned int nBuffer,bool bRenderingShadow);
	
	BEGIN_PROP_MAP(COpenGLModel)
		PROP_FLAGS(m_sFileName,"Archivo",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bLoadBSP,"LoadBSP",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bAutoUpdateBSP,"AutoUpdateBSP",false,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bAutoGenerateBSP,"AutoGenerateBSP",false,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	COpenGLModel(void);
	~COpenGLModel(void);
};
