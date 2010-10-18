// GameRender.h: interface for the GameRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCE_STORE_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_)
#define AFX_RESOURCE_STORE_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MAX_LIGHTS				10
#define MAX_ANIMATIONS			50
#define MAX_ANIMATION_FRAMES	50

#define LIGHT_TYPE_AMBIENT	1
#define LIGHT_TYPE_SPOT		2
#define LIGHT_TYPE_OMNI		3

#define MODEL_TYPE_NONE 0
#define MODEL_TYPE_MAP	1
#define MODEL_TYPE_MDL	2
#define MODEL_TYPE_3DS	3

#include <string>
using namespace std;

class CLight
{
public:
	string		m_sName;
	DWORD		m_dwType;

	CVector		m_vPosition;
	COLORREF	m_cColor;
	float		m_fAttenuationConstant;
	float		m_fAttenuationLinear;
	float		m_fAttenuationQuadratic;

	CVector		m_vSpotDir;
	float		m_fSpotExponent;
	float		m_fSpotAngle;
	unsigned	m_nOpenGlIndex;
	bool		m_bIsActive;

	 CLight(string sName,DWORD dwType,CVector vPos,COLORREF cColor);
	~CLight();
};

class CPreprocessedRenderBuffer
{
public:
	int			nVertexes;
	int			nTriangles;
	GLuint		nBufferObject;
	GLuint		nIndexesBufferObject;
	GLfloat		*pVertexArray;
	GLfloat		*pNormalArray;
	GLfloat		*pTexVertexArray;
	GLfloat		*pTex1VertexArray;
	GLfloat		*pTex2VertexArray;
	GLfloat		*pColorArray;
	GLuint 		*pTriangleVertexIndexes;
	CMaterial	*pMaterial;

	CPreprocessedRenderBuffer();
	~CPreprocessedRenderBuffer();
};

class CFrame
{
public:
	CMaterialPolygon *m_pPolygons;
	int				  m_nPolygons;
	CVector			  m_vMins;
	CVector			  m_vMaxs;

	int							 m_nPreprocessedRenderBuffers;
	CPreprocessedRenderBuffer	*m_pPreprocessedRenderBuffers;

	void CalcBBox();
	void GeneratePreprocessedRenderBuffer();
	void GenerateBufferObjects();
	void GetGeometry(vector<CPolygon *> *pvGeometry);

	CFrame();
	~CFrame();
};

class CModelAnimation
{
public:
	CFrame	*m_pFrames;
	int		 m_nFrames;

	CModelAnimation();
	~CModelAnimation();
};

class CModel
{
public:
	string		 m_sName;
	DWORD		 m_dwType;
	CModelAnimation	*m_pAnimations;
	int			 m_nAnimations;
	CLight		*m_pLights[MAX_LIGHTS];
	int			 m_nLights;
	CModelAnimation *GetAnimation(int nAnimation);
	CFrame			*GetFrame(int nAnimation,int nFrame);
	void Reset();

	CModel();
	~CModel();
};

struct IResourceStore:virtual public ISystemUnknown
{
public:

    virtual void	Init(HDC hDC,HGLRC hRenderContext)=0;
    virtual void	Destroy()=0;

    virtual void AddModel(CModel *pModel)=0;
    virtual void AddMaterial(CMaterial *pMaterial)=0;
    virtual void AddTexture(CTexture *pTexture)=0;

    virtual int         GetMaterialCount()=0;
    virtual CMaterial   *GetMaterialAt(int index)=0;

    virtual CTexture	*LoadTexture(string sFileName,COLORREF *pColorKey=NULL,string *pAlphaFile=NULL,float fOpacity=1.0,CTexture *pExistingTexture=NULL)=0;
    virtual CModel		*LoadModel(string sFileName,CModel *pExistingModel=NULL)=0;

    virtual CMaterial	*FindMaterial(CMaterial *pMaterial)=0;
    virtual CModel		*FindModel(string sFileName)=0;
    virtual CTexture	*FindTexture(string sFileName)=0;
    virtual int			 GetTextureIndex(string sFileName)=0;

    virtual HGLRC	     GetRenderContext()=0;

    virtual ~IResourceStore(){}
};

class CResourceStore: virtual public CSystemObjectBase, virtual public IResourceStore
{
	HDC		m_hDC;
	HGLRC	m_hRenderContext;

	DWORD	m_dwFlags;

	void	FreeTexture(string sFileName);
	void	FreeTextures();
	void	FreeModels();
	void	FreeMaterials();
	void	FreeDetailLevelModels();

	bool	LoadMap(CModel *pModel,string sFileName);
	bool	LoadMdl(CModel *pModel,string sFileName);
	bool	Load3DS(CModel *pModel,string sFileName,DWORD dwFileType);

    map<string,CTexture*>	m_mTextures;
    map<string,CModel*>		m_mModels;
    deque<CMaterial*>		m_lMaterials;

public:

	void		 AddModel(CModel *pModel);
	void		 AddMaterial(CMaterial *pMaterial);
	void		 AddTexture(CTexture *pTexture);
	void		 UpdateTextureBuffers(CTexture *pTexture);
	bool		 ReloadTexture(CTexture *pTexture);

    int		     GetMaterialCount();
    CMaterial   *GetMaterialAt(int index);

	CTexture	*LoadTexture(string sFileName,COLORREF *pColorKey=NULL,string *pAlphaFile=NULL,float fOpacity=1.0,CTexture *pExistingTexture=NULL);
	CModel		*LoadModel(string sFileName,CModel *pExistingModel=NULL);
	bool		 ReloadModel(CModel *pModel);

	void		 ReloadAllTextures();
	void		 ReloadAllModels();
	void		 ReplaceFilePath(string sOldFileName,string sNewFileName);
	
	CMaterial	*FindMaterial(CMaterial *pMaterial);
	CModel		*FindModel(string sFileName);
	CTexture	*FindTexture(string sFileName);
	int			 GetTextureIndex(string sFileName);

    void	Init(HDC hDC,HGLRC hRenderContext);
	void	Destroy();
	void	FreeAllResources();
	HGLRC	GetRenderContext();

	CResourceStore();
	~CResourceStore();
};

bool LoadTextureImageHelper(string sFile,DWORD dwColorType,unsigned *pOpenGLSkinWidth,unsigned *pOpenGLSkinHeight,BYTE **ppBuffer);

#endif // !defined(AFX_RESOURCE_STORE_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_)
