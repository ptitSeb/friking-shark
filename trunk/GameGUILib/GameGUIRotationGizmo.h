#pragma once

class CGameGUIRotationGizmo
{
	bool m_bInteractiveAngles[3];
	bool m_bVisibleAngles[3];

	int  m_nSelectedElement;
	int  m_nSelectionBase;

	double m_dRadius;

	CVector m_vPosition;
	CVector m_vAngles;
	CVector m_vRotationOrigin;
	CVector m_vRotationStartAngles;

	CVector m_vYawAxis1,m_vYawAxis2;
	CVector m_vPitchAxis1,m_vPitchAxis2;
	CVector m_vRollAxis1,m_vRollAxis2;

	void UpdateAxises();
	void RenderAngle(IGenericRender *piRender,IGenericCamera *piCamera,double dAngle,CVector vUp,CVector vRigth,CVector vColor);
	CVector AnglesFromRay(CLine *pMouseRay,IGenericCamera *piCamera);
	
public:

	void 	SetPosition(CVector vPosition);
	void 	SetAngles(CVector vAngles);
	CVector GetAngles();

	void SetRadius(double dRadius);

	void SetInteractiveAngles(bool bYaw,bool bPitch,bool bRoll);
	void SetVisibleAngles(bool bYaw,bool bPitch,bool bRoll);

	void Render(IGenericRender *piRender, IGenericCamera *piCamera);
	int  SelectionRender(int nBaseId,IGenericRender *piRender, IGenericCamera *piCamera);
	void Select(int nElementId);

	bool 	BeginRotation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector	ProcessRotation(CLine *pMouseRay,IGenericCamera *piCamera);
	CVector EndRotation();

	 CGameGUIRotationGizmo();
	~CGameGUIRotationGizmo();
};
