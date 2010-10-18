#pragma once

DECLARE_CUSTOM_WRAPPER1(CFontWrapper,IGenericFont,m_piFont)

class CGameGUILabel: virtual public CGameWindowBase, virtual public IGameGUILabel
{
protected:
	CVector m_vTextColor;
	double  m_dTextAlpha;

	std::string m_sText;
	eTextAlignment m_eHorizontalAlignment;
	eTextAlignment m_eVerticalAlignment;
	bool m_bAutoResizeFont;

	void DrawText(IGenericRender *piRender,CVector &vColor,double dAlpha);

public:

	BEGIN_PROP_MAP(CGameGUILabel)
		PROP_CLASS_CHAIN(CGameWindowBase)
		PROP_VALUE_FLAGS(m_bAutoResizeFont,"AutoResizeFont",false,MRPF_NORMAL|MRPF_OPTIONAL)
			PROP_VALUE_FLAGS(m_vTextColor,"TextColor",CVector(0,0,0),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dTextAlpha,"TextAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_sText,"Text","",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_eHorizontalAlignment,"HorzAlign",eTextAlignment_Left,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_eVerticalAlignment,"VertAlign",eTextAlignment_Top,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP()

	// IGameGUILabel

	void				SetText(std::string sText);
	std::string			GetText();

	void				SetHorizontalAlignment(eTextAlignment eAlignment);
	void				SetVerticalAlignment(eTextAlignment eAlignment);
	eTextAlignment		GetHorizontalAlignment();
	eTextAlignment		GetVerticalAlignment();

	void	GetTextColor(CVector *pvColor,double *pdAlpha);
	void	SetTextColor(CVector vColor,double dAlpha);

	// CGameWindowBase
	void				OnDraw(IGenericRender *piRender);

   CGameGUILabel(void);
	~CGameGUILabel(void);
};
