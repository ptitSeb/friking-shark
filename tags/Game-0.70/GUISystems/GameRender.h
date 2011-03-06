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
#include "GameGUILib.h"

using namespace std;

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


class CGameRender: virtual public CSystemObjectBase,virtual public IGameRender
{
   unsigned int			 m_dwFlags;
 
    unsigned int			 m_dwMaxOpenGLSkinWidth;
    unsigned int			 m_dwMaxOpenGLSkinHeight;

	CEntityManagerWrapper m_EntityManager;
	CPlayAreaManagerWrapper m_PlayAreaManager;
	CWorldManagerWrapper	m_WorldManager;

public:

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	unsigned int	EnableFlags(unsigned int dwFlag);
	unsigned int	DisableFlags(unsigned int dwFlag);
	unsigned int	GetFlag(unsigned int dwFlag);
	unsigned int	ToggleFlag(unsigned int dwFlag);

	void	Render(IGenericRender *piRender,IGenericCamera *piCamera);

	CGameRender();
	~CGameRender();
};

#endif // !defined(AFX_GAMERENDER_H__4441111B_8ADD_4621_AB79_5674F72771BC__INCLUDED_)
