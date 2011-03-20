#pragma once

class CGameGUIBBoxGizmo
{
	int  m_nSelectedElement;
	int  m_nSelectionBase;

	double m_dArrowSize;
	
	CVector m_vMins;
	CVector m_vMaxs;
	CVector m_vCenter;
	CVector m_vPosition;
	CVector m_vTranslationOrigin;
	CVector m_vTranslationStartMins;
	CVector m_vTranslationStartMaxs;
	
	void RenderArrow(IGenericRender *piRender,CVector vPosition,CVector vDirection,CVector vUp,CVector vColor);
	CVector PositionFromRay(CLine *pMouseRay,IGenericCamera *piCamera);
public:

	void SetArrowSize(double dSize);
	
	void SetBounds(CVector vMins,CVector vMaxs);
	void GetBounds(CVector *pvMins,CVector *pvMaxs);
	
	void    SetPosition(CVector vPosition);
	CVector GetPosition();
	
	void Render(IGenericRender *piRender, IGenericCamera *piCamera);
	int  SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera);
	void Select(int nElementId);

	bool    BeginBBox(CLine *pMouseRay,IGenericCamera *piCamera);
	void	ProcessBBox(CLine *pMouseRay,IGenericCamera *piCamera);
	void 	EndBBox();

	 CGameGUIBBoxGizmo();
	~CGameGUIBBoxGizmo();
};
