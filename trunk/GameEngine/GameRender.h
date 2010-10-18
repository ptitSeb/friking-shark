// GameRender.h: interface for the GameRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMERENDER_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_)
#define AFX_GAMERENDER_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_MATERIALS			500
#define MAX_DYNAMIC_LIGHTS		50

#include <string>
using namespace std;
class CRender;
class CDynamicLight:public IDynamicLight
{
	CVector		m_cColor;
	CVector		m_vPosition;
	CVector		m_vMins;
	CVector		m_vMaxs;
	double		m_dRadious;
	int			m_nIndex;
	IEntity		*m_pEntityToTrack;

	friend CRender;

public:
	void	TrackEntity(IEntity *pEntity){m_pEntityToTrack=pEntity;}

	CVector GetColor(){return m_cColor;}
	CVector GetPosition(){return m_vPosition;}
	double  GetRadious(){return m_dRadious;}

	void SetColor(CVector cColor){SetSphericalLight(cColor,m_vPosition,m_dRadious);}
	void SetPosition(CVector vPosition){SetSphericalLight(m_cColor,vPosition,m_dRadious);}
	void SetRadious(double dRadious){SetSphericalLight(m_cColor,m_vPosition,dRadious);}

	void SetSphericalLight(CVector cColor,CVector vPosition,double dRadious);

	CDynamicLight();
};

#define RENDER_ENABLE_AUTO_NORMALS			0x0001
#define RENDER_ENABLE_LIGHTING				0x0002
#define RENDER_ENABLE_FOG					0x0004
#define RENDER_ENABLE_TEXTURES				0x0008
#define RENDER_SHOW_LIGHTS					0x0010
#define RENDER_SHOW_BBOXES					0x0020
#define RENDER_ENABLE_MODEL_LOD				0x0040
#define RENDER_ENABLE_TRANSPARENCY			0x0080
#define RENDER_ENABLE_WORLD_LIGHTING        0x0100
#define RENDER_ENABLE_SOLID					0x0200
#define RENDER_ENABLE_MIPMAPS				0x0400
#define RENDER_SHOW_NORMALS					0x0800


class CRender: virtual public CSystemObjectBase,virtual public IRender
{
    DWORD			 m_dwFlags;
    CLight			*m_pLights[MAX_LIGHTS];
    int				 m_nLights;
    CDynamicLight	*m_pDynamicLights[MAX_DYNAMIC_LIGHTS];
    int				 m_nDynamicLights;
    DWORD			 m_dwMaxOpenGLSkinWidth;
    DWORD			 m_dwMaxOpenGLSkinHeight;

    double			 m_dFogStart;
    double			 m_dFogEnd;
    float			 m_vFogColor[4];

    map<string,CDetailLevelModel *> m_DetailLevelModels;
public:

  void ConfigureFog(double dStart,double dEnd,CVector vColor);

	void ResetLights();
	void ResetDetailLevelModels();

	int		AddModel(CModel *pModel);



	CDetailLevelModel  *GetDetailLevelModel(string sModel);
	void				AddDetailLevelModel(string sModelName,CDetailLevelModel *pDetailLevelModel);


	DWORD	EnableFlags(DWORD dwFlag);
	DWORD	DisableFlags(DWORD dwFlag);
	DWORD	GetFlag(DWORD dwFlag);
	DWORD	ToggleFlag(DWORD dwFlag);

	int		AddLight(CLight *pLight);

	void	Reset();
	void	Render();
	void	RenderWorldShadows();
	void	RenderBBox(CVector vMins,CVector vMaxs);
	void	RenderTextureRect(CTexture *pTexture,CVector vCenter,CVector vAxis1,CVector vAxis2,double s1,double s2);
	void	RenderTextureRect(int nTextureIndex,CVector vCenter,CVector vAxis1,CVector vAxis2,double s1,double s2);
	void	RenderBuffer(CPreprocessedRenderBuffer *pBuffer,CVector vPosition,CVector vAngles,bool bApplyShadows);
	void	RenderBufferNormals(CPreprocessedRenderBuffer *pBuffer);
	void	RenderBufferShadow(CPreprocessedRenderBuffer *pBuffer);

	void	PrepareMaterial(CMaterial *pMaterial);
	void	UnPrepareMaterial(CMaterial *pMaterial);
  void	PrepareOpenGLLighting(IEntity *piEntity);
  void	UnPrepareOpenGLLighting(IEntity *piEntity);

	IDynamicLight  *AllocDynamicLight();
	void			FreeDynamicLight(IDynamicLight *pLight);

	void	ApplyLighting(int nVertexes,float *pVertexArray,float *pVertexNormals,float *pColorArray,CVector &vPosition,CVector &vAngles);
	void	ApplyShadowLighting(int nVertexes,float *pVertexArray,float *pVertexNormals,float *pOriginalColors,float *pColorArray,CVector &vPosition,CVector &vAngles);
	void	ApplyDynamicLightingEntityTracking();

	ICamera *GetCamera(){return NULL;}
	void SetCamera(ICamera *){}

	CRender();
	~CRender();
};

#endif // !defined(AFX_GAMERENDER_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_)
