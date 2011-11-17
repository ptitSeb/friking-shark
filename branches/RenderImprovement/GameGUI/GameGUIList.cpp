//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIList.h"

CGameGUIList::CGameGUIList(void)
{
  	m_bActive=true;
	m_bWantFocus=true;
	m_dThumbPixelsPerLine=0;
	m_nScrollThumbDragStartFirstVisible=0;
	m_nFirstVisible=0;
	m_nVisibleCount=0;
	m_nSelectedElement=-1;
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
  if(nIndex>m_vElements.size()){return;}
  unsigned int x=0;
  std::vector<std::string>::iterator i;
  for(x=0,i=m_vElements.begin();x<m_vElements.size();x++,i++)
  {
	if(x==nIndex){m_vElements.erase(i);break;}
  }
}

void CGameGUIList::SetElement(unsigned long nIndex,std::string sText)
{
	if(nIndex>m_vElements.size()){return;}
	m_vElements[nIndex]=sText;
}

void CGameGUIList::Clear(){ m_vElements.clear();m_nSelectedElement=-1;m_nFirstVisible=0;}
std::string  CGameGUIList::GetElement(unsigned int nElement){if(nElement<m_vElements.size()){return m_vElements[nElement];}return "";}
unsigned int CGameGUIList::GetElementCount(){return m_vElements.size();}
void 		 CGameGUIList::SetSelectedElement(int nElement){m_nSelectedElement=nElement;if(nElement!=-1){ValidateSelection();}}
int 		 CGameGUIList::GetSelectedElement(){return m_nSelectedElement;}

void CGameGUIList::UpdateRealRect()
{
	CGameWindowBase::UpdateRealRect();
	
	double dFontSize=0,dFontPixelWidth=0;
	IGenericFont *piFont=NULL;
	GetFont(&piFont,&dFontSize);
	if(!piFont){return;}
	piFont->CalcTextSize(dFontSize,"A",&dFontPixelWidth,&m_dFontPixelHeight);
	m_nVisibleCount=(int)(m_dFontPixelHeight?m_rRealRect.h/m_dFontPixelHeight:0);
	REL(piFont);
}

void CGameGUIList::OnDraw(IGenericRender *piRender)
{
  CGameWindowBase::OnDraw(piRender);
 
  double dFontSize=0;
  IGenericFont *piFont=NULL;
  GetFont(&piFont,&dFontSize);
  if(!piFont){return;}
  
  double y=m_rRealRect.h;
  double dElementWidth=m_rRealRect.w;
  if(m_nVisibleCount<(int)m_vElements.size())
  {
	dElementWidth=m_rRealRect.w-m_dScrollBarWidth;
	piRender->PushState();
	piRender->ActivateBlending();

	m_dThumbPixelsPerLine=m_vElements.size()?((m_rRealRect.h-m_dScrollBarWidth*2-2*2)/((double)m_vElements.size())):m_rRealRect.h-m_dScrollBarWidth*2-2*2;
	double dThumbSize=((double)m_nVisibleCount)*m_dThumbPixelsPerLine;

	m_rScrollUp.x=dElementWidth;
	m_rScrollUp.y=m_rRealRect.h-m_dScrollBarWidth;
	m_rScrollUp.w=m_dScrollBarWidth;
	m_rScrollUp.h=m_dScrollBarWidth;

	m_rScrollDown.x=dElementWidth;
	m_rScrollDown.y=0;
	m_rScrollDown.w=m_dScrollBarWidth;
	m_rScrollDown.h=m_dScrollBarWidth;

	m_rScroll.x=dElementWidth;
	m_rScroll.y=m_dScrollBarWidth;
	m_rScroll.w=m_dScrollBarWidth;
	m_rScroll.h=m_rRealRect.h-m_dScrollBarWidth*2.0;

	m_rScrollThumb.x=dElementWidth+2;
	m_rScrollThumb.y=(m_rRealRect.h-m_dScrollBarWidth)-dThumbSize-((double)m_nFirstVisible)*m_dThumbPixelsPerLine-2;
	m_rScrollThumb.w=m_dScrollBarWidth-2*2;
	m_rScrollThumb.h=dThumbSize;

	// Boton superior
	piRender->SetColor(m_vScrollButtonColor,1);
	piRender->RenderRect(m_rRealRect.x+m_rScrollUp.x,m_rRealRect.y+m_rScrollUp.y,m_rScrollUp.w,m_rScrollUp.h);
	// ScrollBar
	piRender->SetColor(m_vScrollBkColor,m_dBackgroundAlpha);
	piRender->RenderRect(m_rRealRect.x+m_rScroll.x,m_rRealRect.y+m_rScroll.y,m_rScroll.w,m_rScroll.h);
	// Thumb
	piRender->SetColor(m_vScrollButtonColor,1);
	piRender->RenderRect(m_rRealRect.x+m_rScrollThumb.x,m_rRealRect.y+m_rScrollThumb.y,m_rScrollThumb.w,m_rScrollThumb.h);
	// Boton inferior
	piRender->SetColor(m_vScrollButtonColor,1);
	piRender->RenderRect(m_rRealRect.x+m_rScrollDown.x,m_rRealRect.y+m_rScrollDown.y,m_rScrollDown.w,m_rScrollDown.h);
	piRender->PopState();
  }
  
  for(int x=m_nFirstVisible;x<(m_nFirstVisible+m_nVisibleCount+1) && x<(int)m_vElements.size();x++)
  {
	y-=m_dFontPixelHeight;
	if(m_nSelectedElement==(int)x)
	{
	  piRender->SetColor(m_vSelectedBackgroundColor,m_dTextAlpha);
	  piRender->RenderRect(m_rRealRect.x,m_rRealRect.y+y,dElementWidth,m_dFontPixelHeight);
	  piRender->SetColor(m_vSelectedTextColor,m_dTextAlpha);
	}
	else
	{
	  piRender->SetColor(m_vTextColor,m_dTextAlpha);
	}
	piFont->RenderTextEx(piRender,dFontSize,m_rRealRect.x+1,m_rRealRect.y+y,dElementWidth,m_dFontPixelHeight,m_vElements[x].c_str(),eTextAlignment_Left,eTextAlignment_Center);
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
	std::string sElement=(m_nSelectedElement!=-1)?m_vElements[m_nSelectedElement]:"";
	NOTIFY_EVENT(IGameGUIListEvents,OnSelectionChanged(this,m_nSelectedElement,sElement));
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
  
  SGamePos pos(x,y);
  if(m_nVisibleCount<(int)m_vElements.size() && x>=m_rScroll.x)
  {
	if(m_rScrollUp.Contains(pos)){SelectPrevious();}
	else if(m_rScrollDown.Contains(pos)){SelectNext();}
	else if(m_rScrollThumb.Contains(pos))
	{
		if(DetectDrag(x,y))
		{
			m_piGUIManager->SetMouseCapture(this);
			m_ptScrollThumbDragStart=pos;
			m_nScrollThumbDragStartFirstVisible=m_nFirstVisible;
		}
	}
	else if(m_rScroll.Contains(pos))
	{
		if(y<m_rScrollThumb.y){SelectNextPage();}
		else if(y>(m_rScrollThumb.y+m_rScrollThumb.h)){SelectPreviousPage();}
	}
  }
  else
  {
	m_nSelectedElement=m_nFirstVisible+(int)((m_rRealRect.h-y)/m_dFontPixelHeight);
	if(m_nSelectedElement<0){m_nSelectedElement=-1;}
	if(m_nSelectedElement>=(int)m_vElements.size()){m_nSelectedElement=-1;}
	std::string sElement=(m_nSelectedElement!=-1)?m_vElements[m_nSelectedElement]:"";
	
	NOTIFY_EVENT(IGameGUIListEvents,OnSelectionChanged(this,m_nSelectedElement,sElement));
  }  
}

void CGameGUIList::OnMouseMove(double x,double y)
{
	CGameWindowBase::OnMouseMove(x,y);
	if(m_piGUIManager->HasMouseCapture(this))
	{
		m_nFirstVisible=m_nScrollThumbDragStartFirstVisible-(int)((y-m_ptScrollThumbDragStart.y)/m_dThumbPixelsPerLine);
		if(m_nFirstVisible>=((int)m_vElements.size()-m_nVisibleCount)){m_nFirstVisible=m_vElements.size()-m_nVisibleCount;}
		if(m_nFirstVisible<0){m_nFirstVisible=0;}
	}
}

void CGameGUIList::OnMouseUp(int nButton,double x,double y)
{
	CGameWindowBase::OnMouseUp(nButton,x,y);

	if(m_piGUIManager->HasMouseCapture(this)){m_piGUIManager->ReleaseMouseCapture();}
}


void CGameGUIList::OnMouseDoubleClick(int nButton,double x,double y)
{
  CGameWindowBase::OnMouseDoubleClick(nButton,x,y);

  if(m_nSelectedElement>=0 && m_nSelectedElement<(int)m_vElements.size())
  {
	NOTIFY_EVENT(IGameGUIListEvents,OnSelectionDoubleCliked(this,m_nSelectedElement,m_vElements[m_nSelectedElement]));
  }
}

void CGameGUIList::OnWantFocus(bool *pbWant){*pbWant=m_bWantFocus;}
