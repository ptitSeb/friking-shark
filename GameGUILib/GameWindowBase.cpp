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
#include "GameWindowBase.h"


CGameWindowBase::CGameWindowBase(void)
{
	m_bVisible=false;
	m_bActive=true;
	m_bPopup=false;
	m_bCentered=false;
	m_piParent=NULL;
	m_vBackgroundColor=CVector(0.5,0.5,0.5);
	m_dBackgroundAlpha=1.0;
	m_piGUIManager=NULL;
	m_eReferenceSystem=eGameGUIReferenceSystem_Absolute;
	m_eChildrenLayout=eGameGUIChildrenLayout_None;
	m_dFontSize=0;
	m_bRegisterOnCreation=false;
	m_dSizeInLayout=0;
	m_dLayoutMargin=0;
	m_dLayoutSeparation=0;
	m_bNavigateChildren=false;
}

CGameWindowBase::~CGameWindowBase(void)
{
}

bool CGameWindowBase::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=true;

	m_bPopup=bPopup;
	m_piParent=piParent;
	ADD(m_piParent);
	m_piGUIManager =	piParent->GetGUIManager();
	m_bVisible=(bPopup==false);


	if(m_bPopup)
	{
		m_piGUIManager->AddPopup(this);
	}
	else
	{
		m_piParent->AddChild(this);
	}

	for(unsigned x=0;x<m_vLoadedChildrenList.size();x++)
	{
		IGameWindow *piChild=m_vLoadedChildrenList[x].m_piWindow;
		bResult=piChild->InitWindow(this,false);
		if(bResult==false){break;}
	}

	if(bResult)
	{
		bResult=MapChildren(true,false);
	}

	UpdateRealRect();

	return bResult;
}


void CGameWindowBase::DestroyWindow()
{
	while(m_vChildren.size())
	{
		IGameWindow *piWindow=m_vChildren[0];
		ADD(piWindow);
		piWindow->DestroyWindow(); // DestroyWindow elimina la referencia de m_vChildren al llamar a RemoveChild
		REL(piWindow);
	}

	MapChildren(false,false);

	if(m_piGUIManager)
	{
		if(m_piGUIManager->HasFocus(this))
		{
			m_piGUIManager->SetFocus(m_piParent);
		}
		if(m_piGUIManager->HasMouseCapture(this))
		{
			m_piGUIManager->ReleaseMouseCapture();
		}
	}
	if(m_bPopup)
	{
		if(m_piGUIManager)
		{
			m_piGUIManager->RemovePopup(this);
		}
	}
	else
	{
		if(m_piParent)
		{
			m_piParent->RemoveChild(this);
		}
	}		
	REL(m_piParent);
	REL(m_piGUIManager);
}

void CGameWindowBase::Destroy()
{
	m_BackgroundTexture.Detach();
	m_Font.Detach();

	CSystemObjectBase::Destroy();
}

bool CGameWindowBase::MapChildren(bool bMapping/*false=Unmapping*/,bool bInitialize)
{
	return true;
}

void CGameWindowBase::SetWindowName(std::string sName)
{
	m_sWindowName=sName;
}

std::string CGameWindowBase::GetWindowName()
{
	return m_sWindowName;
}

IGameGUIManager *CGameWindowBase::GetGUIManager()
{
	ADD(m_piGUIManager);
	return m_piGUIManager;
}

bool CGameWindowBase::IsPopup()
{
	return m_bPopup;
}

void CGameWindowBase::Show(bool bShow)
{
	m_bVisible=bShow;
}

void CGameWindowBase::Activate(bool bActive)
{
	m_bActive=bActive;
}

bool CGameWindowBase::IsVisible()
{
	return m_bVisible;
}

bool CGameWindowBase::IsActive()
{
	return m_bActive;
}

void CGameWindowBase::SetReferenceSystem(eGameGUIReferenceSystem eRefSystem)
{
	m_eReferenceSystem=eRefSystem;
}

eGameGUIReferenceSystem CGameWindowBase::GetReferenceSystem()
{
	return m_eReferenceSystem;
}

void CGameWindowBase::GetRect(SGameRect *pRect)
{
	*pRect=m_rRect;
}

void CGameWindowBase::SetRect(SGameRect *pRect)
{
	m_rRect=*pRect;
	UpdateRealRect();
}

void CGameWindowBase::GetRealRect(SGameRect *pRect)
{
	*pRect=m_rRealRect;
}

void CGameWindowBase::UpdateRealRect()
{
	SGameRect rParentRealRect;

	if(m_bPopup)
	{
		IGameWindow *piMainWindow=m_piGUIManager->GetMainWindow();
		piMainWindow->GetRealRect(&rParentRealRect);
		REL(piMainWindow);
	}
	else
	{
		m_piParent->GetRealRect(&rParentRealRect);
	}

	if(m_eReferenceSystem==eGameGUIReferenceSystem_Relative)
	{
		m_rRealRect.x=rParentRealRect.x+m_sMargin.w+rParentRealRect.w*m_rRect.x;
		m_rRealRect.y=rParentRealRect.y+m_sMargin.h+rParentRealRect.h*m_rRect.y;
		m_rRealRect.w=rParentRealRect.w*m_rRect.w-(m_sMargin.w*2.0);
		m_rRealRect.h=rParentRealRect.h*m_rRect.h-(m_sMargin.h*2.0);
	}
	else
	{
		m_rRealRect.x=rParentRealRect.x+m_rRect.x+m_sMargin.w;
		m_rRealRect.y=rParentRealRect.y+m_rRect.y+m_sMargin.h;
		m_rRealRect.w=m_rRect.w-(m_sMargin.w*2.0);
		m_rRealRect.h=m_rRect.h-(m_sMargin.h*2.0);
	}

	if(m_bCentered)
	{
		SGameRect rTempRect;
		rTempRect.w=rParentRealRect.w;
		rTempRect.h=rParentRealRect.h;
		m_rRealRect.CenterOnRect(&rTempRect);
	}
	m_rRealRect.x=round(m_rRealRect.x);
	m_rRealRect.y=round(m_rRealRect.y);
	m_rRealRect.w=round(m_rRealRect.w);
	m_rRealRect.h=round(m_rRealRect.h);
	UpdateChildrenRealRects();
}

void CGameWindowBase::UpdateChildrenRealRects()
{
	if(m_eChildrenLayout==eGameGUIChildrenLayout_Vertical)
	{
		double dVariableSize=m_rRealRect.h-(m_dLayoutMargin*2);
		double dVariableChildCount=0;
		for(unsigned x=0;x<m_vChildren.size();x++)
		{
			if(x!=0){dVariableSize-=m_dLayoutSeparation;}
			IGameWindow *piWindow=m_vChildren[x];
			double dSizeInLayout=piWindow->GetSizeInLayout();
			if(dSizeInLayout>0)
			{
				dVariableSize-=dSizeInLayout;
			}
			else
			{
				dVariableChildCount++;
			}
		}
		if(dVariableSize<0){dVariableSize=0;}
		double dUsedHeight=m_dLayoutMargin;
		SGameRect rChildRealRect;
		for(unsigned x=0;x<m_vChildren.size();x++)
		{
			if(x!=0){dUsedHeight+=m_dLayoutSeparation;}
			IGameWindow *piWindow=m_vChildren[x];
			double dSizeInLayout=piWindow->GetSizeInLayout();
			rChildRealRect.h=(dSizeInLayout>0)?dSizeInLayout:dVariableSize/dVariableChildCount;
			dUsedHeight+=rChildRealRect.h;
			rChildRealRect.x=m_dLayoutMargin;
			rChildRealRect.y=m_rRealRect.h-dUsedHeight;
			rChildRealRect.w=m_rRealRect.w-m_dLayoutMargin*2;
			
			rChildRealRect.x=round(rChildRealRect.x);
			rChildRealRect.y=round(rChildRealRect.y);
			rChildRealRect.w=round(rChildRealRect.w);
			rChildRealRect.h=round(rChildRealRect.h);
			
			piWindow->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
			piWindow->SetRect(&rChildRealRect);
		}
	}
	else if(m_eChildrenLayout==eGameGUIChildrenLayout_Horizontal)
	{
		double dVariableSize=m_rRealRect.w-(m_dLayoutMargin*2);
		double dVariableChildCount=0;
		for(unsigned x=0;x<m_vChildren.size();x++)
		{
			if(x!=0){dVariableSize-=m_dLayoutSeparation;}
			IGameWindow *piWindow=m_vChildren[x];
			double dSizeInLayout=piWindow->GetSizeInLayout();
			if(dSizeInLayout>0)
			{
				dVariableSize-=dSizeInLayout;
			}
			else
			{
				dVariableChildCount++;
			}
		}
		if(dVariableSize<0){dVariableSize=0;}
		double dUsedWidth=m_dLayoutMargin;
		SGameRect rChildRealRect;
		for(unsigned x=0;x<m_vChildren.size();x++)
		{
			if(x!=0){dUsedWidth+=m_dLayoutSeparation;}
			IGameWindow *piWindow=m_vChildren[x];
			double dSizeInLayout=piWindow->GetSizeInLayout();
			dUsedWidth+=rChildRealRect.w;
			rChildRealRect.x=dUsedWidth;
			rChildRealRect.y=m_dLayoutMargin;
			rChildRealRect.w=(dSizeInLayout>0)?dSizeInLayout:dVariableSize/dVariableChildCount;
			rChildRealRect.h=m_rRealRect.h-m_dLayoutMargin*2;
			
			rChildRealRect.x=round(rChildRealRect.x);
			rChildRealRect.y=round(rChildRealRect.y);
			rChildRealRect.w=round(rChildRealRect.w);
			rChildRealRect.h=round(rChildRealRect.h);
			
			piWindow->SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
			piWindow->SetRect(&rChildRealRect);
		}
	}
	else
	{
		for(unsigned x=0;x<m_vChildren.size();x++)
		{
			m_vChildren[x]->UpdateRealRect();
		}
	}
}


void CGameWindowBase::SetFont(IGenericFont *piFont,double dSize)
{
	m_Font.Detach();
	m_Font.Attach(piFont);
	m_dFontSize=dSize;
}

void CGameWindowBase::GetFont(IGenericFont **ppiFont,double *pdSize)
{
	double dParentSize=0;
	if(ppiFont)
	{
	  if(m_Font.m_piFont==NULL)
	  {
		  m_piParent->GetFont(ppiFont,&dParentSize);
	  }
	  else
	  {
		  ADD(m_Font.m_piFont);
		  (*ppiFont)=m_Font.m_piFont;
	  }
	}
	if(pdSize)
	{
	  if(m_dFontSize>0){(*pdSize)=m_dFontSize;}
	  else
	  {m_piParent->GetFont(NULL,pdSize);}
	}
}

IGameWindow *CGameWindowBase::GetParent()
{
	ADD(m_piParent);
	return m_piParent;
}

void CGameWindowBase::GetChildren(std::vector<IGameWindow *> *pvChildren)
{
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vChildrenZOrder.begin();i!=m_vChildrenZOrder.end();i++)
	{
		IGameWindow *piWindow=*i;
		ADD(piWindow);
		pvChildren->push_back(piWindow);
	}
}

void CGameWindowBase::AddChild(IGameWindow *piWindow)
{
	ADD(piWindow);
	m_vChildren.push_back(piWindow);
	m_vChildrenZOrder.push_back(piWindow);
}

void CGameWindowBase::RemoveChild(IGameWindow *piWindow)
{
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vChildren.begin();i!=m_vChildren.end();i++)
	{
		IGameWindow *piTempWindow=*i;
		if(piTempWindow==piWindow)
		{
			m_vChildren.erase(i);
			REL(piTempWindow);
			break;
		}
	}
	for(i=m_vChildrenZOrder.begin();i!=m_vChildrenZOrder.end();i++)
	{
		if(*i==piWindow){m_vChildrenZOrder.erase(i);break;}
	}
}

IGameWindow *CGameWindowBase::FindChild(std::string sName)
{
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vChildren.begin();i!=m_vChildren.end();i++)
	{
		IGameWindow *piWindow=*i;
		if(piWindow->GetWindowName()==sName)
		{
			ADD(piWindow);
			return piWindow;
		}
	}
	for(i=m_vChildren.begin();i!=m_vChildren.end();i++)
	{
		IGameWindow *piWindow=(*i)->FindChild(sName);
		if(piWindow)
		{
			return piWindow;
		}
	}
	return NULL;
}

void CGameWindowBase::BringToFront()
{
	if(m_piParent && !m_bPopup)
	{
		m_piParent->BringChildToFront(this);
	}
}

void CGameWindowBase::BringChildToFront(IGameWindow *piWindow)
{
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vChildrenZOrder.begin();i!=m_vChildrenZOrder.end();i++)
	{
		if(*i==piWindow)
		{
			m_vChildrenZOrder.erase(i);
			m_vChildrenZOrder.push_back(piWindow);
			break;
		}
	}
}

void CGameWindowBase::EnumerateChildren(IGameWindowEnumerationCallback *piCallback)
{
	bool bStopEnumerating=false;
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vChildrenZOrder.begin();i!=m_vChildrenZOrder.end() && !bStopEnumerating;i++)
	{
		piCallback->ProcessEnumeratedWindow(*i,&bStopEnumerating);
	}
}

void CGameWindowBase::OnCharacter(int nCharacter,bool *pbProcessed)
{
}

void CGameWindowBase::GetFocusableDescendants(IGameWindow *piParent,std::vector<IGameWindow *> *pvFocusableWindows)
{
	std::vector<IGameWindow *> vChildren;
	piParent->GetChildren(&vChildren);
	for(unsigned int x=0;x<vChildren.size();x++)
	{
		IGameWindow *piChild=vChildren[x];
		bool bFocusable=false;
		piChild->OnWantFocus(&bFocusable);
		if(bFocusable && piChild->IsActive() && piChild->IsVisible())
		{
			pvFocusableWindows->push_back(piChild);
		}
		else
		{
			GetFocusableDescendants(piChild,pvFocusableWindows);
			REL(piChild);
		}
	}
}

IGameWindow *CGameWindowBase::GetFocusableAncestor()
{
	IGameWindow *piCurrentAncestor=ADD(m_piParent);
	while(piCurrentAncestor)
	{
		bool bFocusable=false;
		piCurrentAncestor->OnWantFocus(&bFocusable);
		if(bFocusable){break;}
		IGameWindow *piNewAncestor=piCurrentAncestor->GetParent();
		REL(piCurrentAncestor);
		piCurrentAncestor=piNewAncestor;
	}
	return piCurrentAncestor;
}

IGameWindow *CGameWindowBase::FindClosestFocusableWindow(IGameWindow *pReference,EFocusableSearchDirection eDirection)
{
	if(pReference==NULL){return FindNextFocusableWindow(NULL);}
	
	IGameWindow *piResult=NULL;
	std::vector<IGameWindow *> vFocusableWindows;
	GetFocusableDescendants(this,&vFocusableWindows);

	SGameRect sRect;
	pReference->GetRealRect(&sRect);
	SGamePos sMid;
	sMid.x=sRect.x+sRect.w*0.5;
	sMid.y=sRect.y+sRect.h*0.5;
	double dCurrentDistance=0;
	
	unsigned int x=0;
	for(x=0;x<vFocusableWindows.size();x++)
	{
		IGameWindow *piWindow=vFocusableWindows[x];
		if(pReference==piWindow)
		{
			REL(piWindow);
			continue;
		}
		
		SGameRect sCandidateRect;
		piWindow->GetRealRect(&sCandidateRect);
		SGamePos sCandidateMid;
		sCandidateMid.x=sCandidateRect.x+sCandidateRect.w*0.5;
		sCandidateMid.y=sCandidateRect.y+sCandidateRect.h*0.5;
		
		bool bHorzOverlap=(sCandidateMid.y>=sRect.y && sCandidateMid.y<=sRect.y+sRect.h)||
						  (sMid.y>=sCandidateRect.y && sMid.y<=sCandidateRect.y+sCandidateRect.h);
		
		bool bValidCandidate=false;
		switch(eDirection)
		{
			case eFocusableSearchLeft:  bValidCandidate=(sCandidateMid.x<sMid.x && bHorzOverlap);break;
			case eFocusableSearchRight: bValidCandidate=(sCandidateMid.x>sMid.x && bHorzOverlap);break;
			case eFocusableSearchUp:    bValidCandidate=(sCandidateMid.y>sMid.y && !bHorzOverlap);break;
			case eFocusableSearchDown:  bValidCandidate=(sCandidateMid.y<sMid.y && !bHorzOverlap);break;
		};
		double dDist=CVector(sCandidateMid.x,sCandidateMid.y,0)-CVector(sMid.x,sMid.y,0);
		if(bValidCandidate && (piResult==NULL || dDist<dCurrentDistance))
		{
			if(piResult){REL(piResult);}
			
			dCurrentDistance=dDist;
			piResult=ADD(piWindow);
		}		
		REL(piWindow);
	}
	
	if(piResult)
	{
		return piResult;
	}
	else
	{
		return ADD(pReference);
	}
}

IGameWindow *CGameWindowBase::FindNextFocusableWindow(IGameWindow *pReference)
{
	IGameWindow *piResult=NULL;
	bool bReferenceWasPrevious=false;
	std::vector<IGameWindow *> vFocusableWindows;
	GetFocusableDescendants(this,&vFocusableWindows);
	
	unsigned int x=0;
	for(x=0;x<vFocusableWindows.size();x++)
	{
		IGameWindow *piWindow=vFocusableWindows[x];
		if(pReference==piWindow)
		{
			bReferenceWasPrevious=true;
		}
		else if(piResult==NULL && (pReference==NULL || bReferenceWasPrevious))
		{
			bReferenceWasPrevious=false;
			piResult=ADD(piWindow);
		}		
		REL(piWindow);
	}
	return piResult;
}

IGameWindow *CGameWindowBase::FindPreviousFocusableWindow(IGameWindow *pReference)
{
	IGameWindow *piResult=NULL;
	std::vector<IGameWindow *> vFocusableWindows;
	GetFocusableDescendants(this,&vFocusableWindows);
	bool bReferenceWasNext=false;
	int x=0;
	for(x=vFocusableWindows.size()-1;x>=0;x--)
	{
		IGameWindow *piWindow=vFocusableWindows[x];
		if(pReference==piWindow)
		{
			bReferenceWasNext=true;
		}
		else if(piResult==NULL && (pReference==NULL || bReferenceWasNext))
		{
			bReferenceWasNext=false;
			piResult=ADD(piWindow);
		}		
		REL(piWindow);
	}
	return piResult;
}

IGameWindow *CGameWindowBase::GetFocusedDescendant()
{
	IGameWindow *piResult=NULL;
	std::vector<IGameWindow *> vFocusableWindows;
	GetFocusableDescendants(this,&vFocusableWindows);
	
	unsigned int x=0;
	for(x=0;x<vFocusableWindows.size();x++)
	{
		IGameWindow *piWindow=vFocusableWindows[x];
		if(m_piGUIManager->HasFocus(piWindow))
		{
			piResult=ADD(piWindow);
		}
		REL(piWindow);
	}
	return piResult;
}

void CGameWindowBase::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(m_bNavigateChildren)
	{
		if(nKey==GK_HOME)
		{
			*pbProcessed=true;
			IGameWindow *piNew=FindNextFocusableWindow(NULL);
			if(piNew){m_piGUIManager->SetFocus(piNew);}
			REL(piNew);
		}
		else if(nKey==GK_END)
		{
			*pbProcessed=true;
			IGameWindow *piNew=FindPreviousFocusableWindow(NULL);
			if(piNew){m_piGUIManager->SetFocus(piNew);}
			REL(piNew);
		}
		else if(nKey==GK_RIGHT || nKey==GK_DOWN)
		{
			*pbProcessed=true;
			IGameWindow *piCurrentFocusedWindow=GetFocusedDescendant();
			IGameWindow *piNew=FindClosestFocusableWindow(piCurrentFocusedWindow,nKey==GK_RIGHT?eFocusableSearchRight:eFocusableSearchDown);
			if(piNew==NULL){piNew=FindNextFocusableWindow(NULL);}
			if(piNew){m_piGUIManager->SetFocus(piNew);}
			REL(piNew);
			REL(piCurrentFocusedWindow);
		}
		else if(nKey==GK_LEFT || nKey==GK_UP)
		{
			*pbProcessed=true;
			IGameWindow *piCurrentFocusedWindow=GetFocusedDescendant();
			IGameWindow *piNew=FindClosestFocusableWindow(piCurrentFocusedWindow,nKey==GK_LEFT?eFocusableSearchLeft:eFocusableSearchUp);
			if(piNew==NULL){piNew=FindPreviousFocusableWindow(NULL);}
			if(piNew){m_piGUIManager->SetFocus(piNew);}
			REL(piNew);
			REL(piCurrentFocusedWindow);
		}
		else if(nKey=='\t' && (m_piGUIManager->IsKeyDown(GK_LSHIFT) || m_piGUIManager->IsKeyDown(GK_RSHIFT)))
		{
			*pbProcessed=true;
			IGameWindow *piCurrentFocusedWindow=GetFocusedDescendant();
			IGameWindow *piNew=FindPreviousFocusableWindow(piCurrentFocusedWindow);
			if(piNew==NULL){piNew=FindPreviousFocusableWindow(NULL);}
			if(piNew){m_piGUIManager->SetFocus(piNew);}
			REL(piNew);
			REL(piCurrentFocusedWindow);
		}
		else if(nKey=='\t' && !m_piGUIManager->IsKeyDown(GK_LSHIFT) && !m_piGUIManager->IsKeyDown(GK_RSHIFT))
		{
			*pbProcessed=true;
			IGameWindow *piCurrentFocusedWindow=GetFocusedDescendant();
			IGameWindow *piNew=FindNextFocusableWindow(piCurrentFocusedWindow);
			if(piNew==NULL){piNew=FindNextFocusableWindow(NULL);}
			if(piNew){m_piGUIManager->SetFocus(piNew);}
			REL(piNew);
			REL(piCurrentFocusedWindow);
		}
	}
}
void CGameWindowBase::OnKeyUp(int nKey,bool *pbProcessed)
{
}

void CGameWindowBase::OnMouseDown(int nButton,double x,double y){}
void CGameWindowBase::OnMouseUp(int nButton,double x,double y){}
void CGameWindowBase::OnMouseMove(double x,double y){}
void CGameWindowBase::OnMouseDoubleClick(int nButton,double x,double y){}
void CGameWindowBase::OnMouseWheelUp(double x,double y){}
void CGameWindowBase::OnMouseWheelDown(double x,double y){}

void CGameWindowBase::OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed)
{
	if(m_MouseCursorTexture.m_piTexture)
	{
		unsigned dWidth=0,dHeight=0;
		m_MouseCursorTexture.m_piTexture->GetSize(&dWidth,&dHeight);
		piRender->SelectTexture(m_MouseCursorTexture.m_piTexture,0);
		piRender->RenderTexture(CVector(position.x+dWidth*0.5,position.y-dHeight*0.5,0),dWidth,dHeight,ColorWhite,1);
		piRender->UnselectTexture(0);
		(*pbDrawed)=true;
	}
}

void CGameWindowBase::OnDrawBackground(IGenericRender *piRender)
{
	if(m_BackgroundTexture.m_piTexture)
	{
		piRender->SelectTexture(m_BackgroundTexture.m_piTexture,0);
		piRender->RenderTexture(CVector(m_rRealRect.x+m_rRealRect.w*0.5,m_rRealRect.y+m_rRealRect.h*0.5,0),m_rRealRect.w,m_rRealRect.h,m_vBackgroundColor,m_dBackgroundAlpha);
		piRender->UnselectTexture(0);
	}
	else
	{
		if(m_dBackgroundAlpha!=0.0)
		{
			piRender->RenderRect(m_rRealRect.x,m_rRealRect.y,m_rRealRect.w,m_rRealRect.h,m_vBackgroundColor,m_dBackgroundAlpha);
		}
	}

	if(m_BackgroundModel.m_piModel)
	{
		CVector vMaxs,vMins;
		m_BackgroundModel.m_piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
		CVector vSize=vMaxs-vMins;
		CVector vCenter=(vMaxs+vMins)*0.5;
		vCenter.c[1]=vMaxs.c[1]+1;
		piRender->ClearDepth();
		piRender->ActivateClipping();
		piRender->ActivateDepth();
		piRender->SetClipRect(m_rRealRect.x,m_rRealRect.y,m_rRealRect.w,m_rRealRect.h);
		piRender->SetOrthographicProjection(vSize.c[0],vSize.c[2]);
		piRender->SetCamera(vCenter,0,-90,0);
		piRender->SetViewport(m_rRealRect.x,m_rRealRect.y,m_rRealRect.w,m_rRealRect.h);
		piRender->RenderModel(Origin,Origin,m_BackgroundModel.m_piModel);
		piRender->DeactivateDepth();
		m_piGUIManager->RestoreViewport();
		piRender->DeactivateClipping();
	}
}
void CGameWindowBase::OnDraw(IGenericRender *piRender){}

void CGameWindowBase::OnSetFocus(){}
void CGameWindowBase::OnKillFocus(IGameWindow *piFocusedWindow){}
void CGameWindowBase::OnReleaseMouseCapture(){}
void CGameWindowBase::OnWantFocus(bool *pbWant){*pbWant=m_bNavigateChildren;}

IGenericTexture *CGameWindowBase::GetBackgroundTexture()
{
	IGenericTexture *piTexture=m_BackgroundTexture.m_piTexture;
	ADD(piTexture);
	return piTexture;
}

void CGameWindowBase::SetBackgroundTexture(IGenericTexture *piTexture)
{
	m_BackgroundTexture.Detach();
	if(piTexture)
	{
		m_BackgroundTexture.Attach(piTexture);
	}
}

IGenericModel *CGameWindowBase::GetBackgroundModel()
{
	IGenericModel *piModel=m_BackgroundModel.m_piModel;
	ADD(piModel);
	return piModel;
}

void CGameWindowBase::SetBackgroundModel(IGenericModel *piModel)
{
	m_BackgroundModel.Detach();
	if(piModel)
	{
		m_BackgroundModel.Attach(piModel);
	}
}

void	CGameWindowBase::GetBackgroundColor(CVector *pvColor,double *pdAlpha)
{
	*pvColor=m_vBackgroundColor;
	*pdAlpha=m_dBackgroundAlpha;
}

void	CGameWindowBase::SetBackgroundColor(CVector vColor,double dAlpha)
{
	m_vBackgroundColor=vColor;
	m_dBackgroundAlpha=dAlpha;
}

bool CGameWindowBase::OpenFileDialog(std::string sTitle,const char *psFilter,std::string *psFile)
{	
	CFileDialogWrapper dialog;
	dialog.Attach("GameGUI","FileDialog");
	bool bOk=false;
	if(dialog.m_piFileDialog){bOk=dialog.m_piFileDialog->OpenFile(this,sTitle,psFilter,psFile);}
	return bOk;
}

bool CGameWindowBase::SaveFileDialog(std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn)
{
	CFileDialogWrapper dialog;
	dialog.Attach("GameGUI","FileDialog");
	bool bOk=false;
	if(dialog.m_piFileDialog){bOk=dialog.m_piFileDialog->SaveFile(this,sTitle,psFilter,psFile,bOverWriteWarn);}
	return bOk;
}

bool CGameWindowBase::SelectColorDialog(std::string sTitle,CVector *pvColor)
{
	CColorDialogWrapper dialog;
	dialog.Attach("GameGUI","ColorDialog");
	bool bOk=false;
	if(dialog.m_piColorDialog){bOk=dialog.m_piColorDialog->SelectColor(this,sTitle,pvColor);}
	return bOk;
}

bool CGameWindowBase::SelectColorDialog(std::string sTitle,IGameGUIColorDialogCallback *piCallBack,CVector *pvColor)
{
	CColorDialogWrapper dialog;
	dialog.Attach("GameGUI","ColorDialog");
	bool bOk=false;
	if(dialog.m_piColorDialog){bOk=dialog.m_piColorDialog->SelectColor(this,piCallBack,sTitle,pvColor);}
	return bOk;
}

bool CGameWindowBase::ConfirmDialog(std::string sText,std::string sTitle,EMessageDialogType eType)
{	
	CConfirmDialogWrapper dialog;
	dialog.Attach("GameGUI","ConfirmDialog");
	bool bOk=false;
	if(dialog.m_piConfirmDialog){bOk=dialog.m_piConfirmDialog->Confirm(this,sText,sTitle,eType);}
	return bOk;
}

bool CGameWindowBase::InputDialog(std::string *psText,std::string sTitle)
{	
	CInputDialogWrapper dialog;
	dialog.Attach("GameGUI","InputDialog");
	bool bOk=false;
	if(dialog.m_piInputDialog){bOk=dialog.m_piInputDialog->ShowInput(this,psText,sTitle);}
	return bOk;
}

void CGameWindowBase::MessageDialog(std::string sText,std::string sTitle,EMessageDialogType eType)
{
	CMessageDialogWrapper dialog;
	dialog.Attach("GameGUI","MessageDialog");
	if(dialog.m_piMessageDialog){dialog.m_piMessageDialog->ShowMessage(this,sText,sTitle,eType);}
}

bool CGameWindowBase::DetectDrag(double dx,double dy)
{
  if(!m_piGUIManager){return false;}
  return m_piGUIManager->DetectDrag(m_rRealRect.x+dx,m_rRealRect.y+dy);
}

bool CGameWindowBase::Unserialize( ISystemPersistencyNode *piNode )
{
	bool bOk=CSystemObjectBase::Unserialize(piNode);
	if(bOk && m_bRegisterOnCreation)
	{
		CGameGUIManagerWrapper guiManager;
		guiManager.Attach("GameGUI","GUIManager");
		IGameWindow *piMainWindow=guiManager.m_piInterface->GetMainWindow();
		InitWindow(piMainWindow,false);
		Show(true);
		REL(piMainWindow);
	}
	return bOk;
}

double  CGameWindowBase::GetSizeInLayout(){return m_dSizeInLayout;}
void    CGameWindowBase::SetSizeInLayout(double dSize){m_dSizeInLayout=dSize;}

CLine CGameWindowBase::GetMouseRay(double x,double y,double dLength,IGenericCamera *piCamera)
{
	double dAspect=piCamera->GetAspectRatio();
	double dRightDisplace=(x-m_rRealRect.w*0.5)/(m_rRealRect.w*0.5);
	double dUpDisplace=(y-m_rRealRect.h*0.5)/(m_rRealRect.h*0.5);

	double dProjPlaneW=tan(DegreesToRadians(piCamera->GetViewAngle()*0.5))*dLength*dAspect;
	double dProjPlaneH=tan(DegreesToRadians(piCamera->GetViewAngle()*0.5))*dLength;

	double dProjPlaneRight=dProjPlaneW*dRightDisplace;
	double dProjPlaneUp=dProjPlaneH*dUpDisplace;

	CVector vProjPlaneCenter=piCamera->GetPosition()+piCamera->GetForwardVector()*dLength;
	CVector vProjPlanePos=vProjPlaneCenter+piCamera->GetRightVector()*dProjPlaneRight+piCamera->GetUpVector()*dProjPlaneUp;

	CLine line;
	line.m_Points[0]=piCamera->GetPosition();
	line.m_Points[1]=vProjPlanePos;
	return line;
}

void CGameWindowBase::ComputeClipRect(SGameRect *pRect)
{
	*pRect=m_rRealRect;	
	if(m_piParent)
	{
		SGameRect sParentClipRect;
		m_piParent->ComputeClipRect(&sParentClipRect);
		pRect->ClipToRect(&sParentClipRect);
	}
}
