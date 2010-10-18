#pragma once

#include "PlayAreaElements.h"

class CScenarioProgressBar: virtual public CGameWindowBase,virtual public IPlayAreaElementEnumerationCallback
{
	CPlayAreaManagerWrapper m_PlayAreaManagerWrapper;
	CGenericTextureWrapper m_ThumbTexture;
	CGenericTextureWrapper m_CompleteTexture;
	CGenericTextureWrapper m_CheckPointTexture;
	CGenericTextureWrapper m_CheckPointCompleteTexture;

	CGameWindowWrapper		m_ThumbWindowWrapper;

	vector<IPlayAreaCheckPoint *> m_vCheckPoints;

	double m_dVerticalMargin;

public:

	BEGIN_PROP_MAP(CScenarioProgressBar)
		PROP_CLASS_CHAIN(CGameWindowBase)
		PROP_FLAGS(m_ThumbTexture,"ThumbTexture",MRPF_NORMAL);
		PROP_FLAGS(m_CompleteTexture,"CompleteTexture",MRPF_NORMAL);
		PROP_FLAGS(m_CheckPointTexture,"CheckPointTexture",MRPF_NORMAL);
		PROP_FLAGS(m_CheckPointCompleteTexture,"CheckPointCompleteTexture",MRPF_NORMAL);
		PROP_VALUE_FLAGS(m_dVerticalMargin,"VerticalMargin",0,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP()

	bool InitWindow(IGameWindow *piParent,bool bPopup);
	void DestroyWindow();

	void OnDrawBackground(IGenericRender *piRender);

	// IPlayAreaElementEnumerationCallback

	void ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating);

	 CScenarioProgressBar(void);
	~CScenarioProgressBar(void);
};
