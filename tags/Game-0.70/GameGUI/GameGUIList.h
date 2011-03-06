#pragma once

class CGameGUIList: virtual public CGameWindowBase,virtual public IGameGUIList
{
	std::vector<std::string> m_vElements;
	
	CVector m_vTextColor;
	double  m_dTextAlpha;
	CVector m_vSelectedTextColor;
	CVector m_vSelectedBackgroundColor;
	CVector m_vScrollBkColor;
	CVector m_vScrollButtonColor;
	int 	m_nSelectedElement;

	double    m_dThumbPixelsPerLine;
	SGamePos  m_ptScrollThumbDragStart;
	int       m_nScrollThumbDragStartFirstVisible;
	SGameRect m_rScrollUp;
	SGameRect m_rScrollDown;
	SGameRect m_rScroll;
	SGameRect m_rScrollThumb;

	double  m_dScrollBarWidth;
	
	int 	m_nFirstVisible;
	int 	m_nVisibleCount;
	int 	m_nMouseWheelPageSize;
	double  m_dFontPixelHeight;
	
	void SelectNext();
	void SelectNextPage();
	void SelectPrevious();
	void SelectPreviousPage();
	void ValidateSelection();
	
public:
  
	BEGIN_PROP_MAP(CGameGUIList)
		PROP_CLASS_CHAIN(CGameWindowBase)
		PROP_VALUE_FLAGS(m_vTextColor,"TextColor",CVector(1,1,1),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dTextAlpha,"TextAlpha",1.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vSelectedTextColor,"SelectedTextColor",CVector(1,1,1),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vSelectedBackgroundColor,"SelectedBKColor",CVector(0,0,1),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_vElements,"Elements",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nSelectedElement,"Selected",-1,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dScrollBarWidth,"ScrollWidth",15.0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vScrollBkColor,"ScrollBkColor",CVector(0.4,0.4,0.4),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vScrollButtonColor,"ScrollButtonColor",CVector(0.7,0.7,0.7),MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nMouseWheelPageSize,"MouseWheelPageSize",3,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()
	
	void DestroyWindow();
	
	void OnDraw(IGenericRender *piRender);
	void OnKeyDown(int nKey,bool *pbProcessed);
	void OnMouseDown(int nButton,double x,double y);
	void OnMouseMove(double x,double y);
	void OnMouseUp(int nButton,double x,double y);
	void OnMouseWheelDown(double x,double y);
	void OnMouseWheelUp(double x,double y);
	void OnMouseDoubleClick(int nButton,double x,double y);
	void OnWantFocus(bool *pbWant);

	//IGameGUIList
	
	unsigned long AddElement(std::string sText);
	void		  RemoveElement(unsigned long nIndex);
	void		  Clear();
	
	std::string  GetElement(unsigned int nElement);
	unsigned int GetElementCount();
	
	void 		 SetSelectedElement(int nElement);
	int 		 GetSelectedElement();
	
	CGameGUIList(void);
	~CGameGUIList(void);
};
