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

#include <string>
using namespace std;

class CFrame
{
public:
	CMaterialPolygon *m_pPolygons;
	int				  m_nPolygons;
	CVector			  m_vMins;
	CVector			  m_vMaxs;


	void CalcBBox();
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

	CModelAnimation *GetAnimation(int nAnimation);
	CFrame			*GetFrame(int nAnimation,int nFrame);
	void Reset();

	CModel();
	~CModel();
};

class CResourceStore: virtual public CSystemObjectBase
{
	DWORD	m_dwFlags;

	void	FreeModels();
	void	FreeMaterials();

	bool	LoadASE(CModel *pModel,string sFileName);

    map<string,CModel*>		m_mModels;
    deque<CMaterial*>		m_lMaterials;

public:

	void		 AddModel(CModel *pModel);
	void		 AddMaterial(CMaterial *pMaterial);

    int		     GetMaterialCount();
    CMaterial   *GetMaterialAt(int index);

	CModel		*LoadModel(string sFileName,CModel *pExistingModel=NULL);
	
	CMaterial	*FindMaterial(CMaterial *pMaterial);
	CModel		*FindModel(string sFileName);

    void	Init();
	void	Destroy();
	void	FreeAllResources();

	CResourceStore();
	~CResourceStore();
};

#endif // !defined(AFX_RESOURCE_STORE_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_)
