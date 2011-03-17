#pragma once

class CGameGUITranslationGizmo
{
	bool m_bInteractiveAxises[3];
	bool m_bVisibleAxises[3];

	int  m_nSelectedElement;
	int  m_nSelectionBase;

	double m_dSize;

	CVector m_vPosition;
	CVector m_vTranslationOrigin;
	CVector m_vTranslationStartPosition;

	void RenderAxis(IGenericRender *piRender,CVector vDir,CVector vUp,CVector vColor);
	void RenderAxisPlane(IGenericRender *piRender,CVector vDir,CVector vUp,CVector vColor);
	CVector	PositionFromRay(CLine *pMouseRay,IGenericCamera *piCamera);
public:

	void 	SetPosition(CVector vPosition);
	CVector GetPosition();

	void SetSize(double dSize);

	void SetInteractiveAxises(bool bX,bool bY,bool bZ);
	void SetVisibleAxises(bool bX,bool bY,bool bZ);

	void Render(IGenericRender *piRender, IGenericCamera *piCamera);
	int  SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera);
	void Select(int nElementId);

	bool 	BeginTranslation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector	ProcessTranslation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector EndTranslation();

	 CGameGUITranslationGizmo();
	~CGameGUITranslationGizmo();
};
