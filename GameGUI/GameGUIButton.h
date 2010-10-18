#pragma once

class CGameGUIButton: virtual public CGameGUILabel, virtual public IGameGUIButton
{
	bool m_bClickInProgress;

	CGenericTextureWrapper m_HoverTexture;
	CVector		m_vHoverBackgroundColor;
	double		m_dHoverBackgroundAlpha;
	CVector		m_vHoverTextColor;
	double		m_dHoverTextAlpha;
	bool		m_bHoverEnabled;

protected:

	BEGIN_PROP_MAP(CGameGUIButton)
		PROP_CLASS_CHAIN(CGameGUILabel)
		PROP_FLAGS(m_HoverTexture,"HoverTexture",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bHoverEnabled,"HoverEnabled",true,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dHoverBackgroundAlpha,"HoverBkAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vHoverBackgroundColor,"HoverBkColor",CVector(0.5,0.5,1.0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dHoverTextAlpha,"HoverTextAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vHoverTextColor,"HoverTextColor",CVector(0,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

	void OnMouseMove(double x,double y);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseUp(int nButton,double x,double y);

public:
	// IGameGUIButton

	void OnDrawBackground(IGenericRender *piRender);
	void OnDraw(IGenericRender *piRender);


	CGameGUIButton(void);
	~CGameGUIButton(void);
};
