#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIList.h"

CGameGUIList::CGameGUIList(void)
{
	m_nFirstVisible=0;
	m_nVisibleCount=0;
	m_dFontPixelHeight=0;
	PersistencyDefaultValue();
}

CGameGUIList::~CGameGUIList(void)
{
}

void CGameGUIList::DestroyWindow()
{
  Clear();
  CGameWindowBase::DestroyWindow();
}

unsigned long CGameGUIList::AddElement(std::string sText){m_vElements.push_back(sText);return m_vElements.size()-1;}
void          CGameGUIList::RemoveElement(unsigned long nIndex)
{
  unsigned int x=0;
  std::vector<std::string>::iterator i;
  for(x=0,i=m_vElements.begin();x<m_vElements.size();x++,i++)
  {
	if(x==nIndex){m_vElements.erase(i);break;}
  }
}

void CGameGUIList::Clear(){ m_vElements.clear();}
std::string  CGameGUIList::GetElement(unsigned int nElement){if(nElement<m_vElements.size()){return m_vElements[nElement];}return "";}
unsigned int CGameGUIList::GetElementCount(){return m_vElements.size();}
void 		 CGameGUIList::SetSelectedElement(unsigned int nElement){if(nElement<m_vElements.size()){m_nSelectedElement=nElement;}}
unsigned int CGameGUIList::GetSelectedElement(){return m_nSelectedElement;}


void CGameGUIList::OnDraw(IGenericRender *piRender)
{
  CGameWindowBase::OnDraw(piRender);
 
  double dFontSize=0,dFontPixelWidth=0;
  
  IGenericFont *piFont=NULL;
  GetFont(&piFont,&dFontSize);
  if(!piFont){return;}
  piFont->CalcTextSize(dFontSize,"A",&dFontPixelWidth,&m_dFontPixelHeight);

  double y=m_rRealRect.h;
  m_nVisibleCount=m_dFontPixelHeight?m_rRealRect.h/m_dFontPixelHeight:0;
  double dElementWidth=m_rRealRect.w;
  if(m_nVisibleCount<(int)m_vElements.size())
  {
	dElementWidth=m_rRealRect.w-m_dScrollWidth;
	piRender->PushState();
	piRender->ActivateBlending();
	// Boton superior
	piRender->SetColor(m_vScrollButtonColor,1);
	piRender->RenderRect(dElementWidth, m_rRealRect.h-m_dScrollWidth,m_dScrollWidth,m_dScrollWidth);
	// ScrollBar
	piRender->SetColor(m_vScrollBkColor,m_dBackgroundAlpha);
	piRender->RenderRect(dElementWidth, m_dScrollWidth              ,m_dScrollWidth,m_rRealRect.h-m_dScrollWidth*2.0);
	// Thumb
	piRender->SetColor(m_vScrollButtonColor,1);
	double dThumbPixelsPerLine=m_vElements.size()?((m_rRealRect.h-m_dScrollWidth*2-2*2)/((double)m_vElements.size())):m_rRealRect.h-m_dScrollWidth*2-2*2;
	double dThumbSize=((double)m_nVisibleCount)*dThumbPixelsPerLine;
	double dThumbPos=(m_rRealRect.h-m_dScrollWidth)-dThumbSize-((double)m_nFirstVisible)*dThumbPixelsPerLine-2;
	piRender->RenderRect(dElementWidth+2,dThumbPos                  ,m_dScrollWidth-2*2,dThumbSize);
	// Boton inferior
	piRender->SetColor(m_vScrollButtonColor,1);
	piRender->RenderRect(dElementWidth, 0                           ,m_dScrollWidth,m_dScrollWidth);
	piRender->PopState();
  }
  
  for(int x=m_nFirstVisible;x<(m_nFirstVisible+m_nVisibleCount+1) && x<(int)m_vElements.size();x++)
  {
	y-=m_dFontPixelHeight;
	if(m_nSelectedElement==(int)x)
	{
	  piRender->SetColor(m_vSelectedBackgroundColor,m_dTextAlpha);
	  piRender->RenderRect(0,y,dElementWidth,m_dFontPixelHeight);
	  piRender->SetColor(m_vSelectedTextColor,m_dTextAlpha);
	}
	else
	{
	  piRender->SetColor(m_vTextColor,m_dTextAlpha);
	}
	piFont->RenderTextEx(dFontSize,1,y,dElementWidth,m_dFontPixelHeight,m_vElements[x].c_str(),eTextAlignment_Left,eTextAlignment_Center);
  }
  
  REL(piFont);
}

void CGameGUIList::OnKeyDown(int nKey,bool *pbProcessed)
{
  CGameWindowBase::OnKeyDown(nKey,pbProcessed);
  
  if(nKey==GK_UP){SelectPrevious();*pbProcessed=true;}
  if(nKey==GK_DOWN){SelectNext();*pbProcessed=true;}
  if(nKey==GK_PAGEDOWN){SelectNextPage();*pbProcessed=true;}
  if(nKey==GK_PAGEUP){SelectPreviousPage();*pbProcessed=true;}
  if(nKey==GK_HOME){m_nSelectedElement=0;ValidateSelection();*pbProcessed=true;}
  if(nKey==GK_END){m_nSelectedElement=m_vElements.size()-1;ValidateSelection();*pbProcessed=true;}
}

void CGameGUIList::SelectNext(){m_nSelectedElement++;ValidateSelection();}
void CGameGUIList::SelectPrevious(){m_nSelectedElement--;ValidateSelection();}
void CGameGUIList::SelectNextPage(){m_nSelectedElement+=m_nVisibleCount;ValidateSelection();}
void CGameGUIList::SelectPreviousPage(){m_nSelectedElement-=m_nVisibleCount;ValidateSelection();}

void CGameGUIList::ValidateSelection()
{
	if(m_nSelectedElement<0){m_nSelectedElement=0;}
	if(m_nSelectedElement>=(int)m_vElements.size()){m_nSelectedElement=m_vElements.size()-1;}
	if(m_nSelectedElement<m_nFirstVisible){m_nFirstVisible=m_nSelectedElement;}
	if(m_nSelectedElement>=(m_nFirstVisible+m_nVisibleCount))
	{
	  m_nFirstVisible=(m_nSelectedElement-(m_nVisibleCount-1));
	}
}

void CGameGUIList::OnMouseWheelDown(double x,double y)
{
  m_nFirstVisible+=m_nMouseWheelPageSize;
  if(m_nFirstVisible>=((int)m_vElements.size()-m_nVisibleCount)){m_nFirstVisible=m_vElements.size()-m_nVisibleCount;}
  if(m_nFirstVisible<0){m_nFirstVisible=0;}
}

void CGameGUIList::OnMouseWheelUp(double x,double y)
{
  m_nFirstVisible-=m_nMouseWheelPageSize;
  if(m_nFirstVisible<0){m_nFirstVisible=0;}
}

void CGameGUIList::OnMouseDown(int nButton,double x,double y)
{
  CGameWindowBase::OnMouseDown(nButton,x,y);
  
  if(m_nVisibleCount<(int)m_vElements.size() && x>=(m_rRealRect.w-m_dScrollWidth))
  {
	// Top Button
	if(y>=(m_rRealRect.h-m_dScrollWidth))
	{
	  SelectPrevious();
	}
	// Bottom Button
	else if(y<=m_dScrollWidth)
	{
	  SelectNext();
	}
	else // Scroll bar
	{
	}
  }
  else
  {
	m_nSelectedElement=m_nFirstVisible+((m_rRealRect.h-y)/m_dFontPixelHeight);
	if(m_nSelectedElement<0){m_nSelectedElement=-1;}
	if(m_nSelectedElement>(int)m_vElements.size()){m_nSelectedElement=-1;}
  }
}







