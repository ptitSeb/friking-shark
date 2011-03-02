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
	m_dFontSize=0;
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
		bResult=MapChildren(true);
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

	MapChildren(false);

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

bool CGameWindowBase::MapChildren(bool bMapping/*false=Unmapping*/)
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
		m_rRealRect.x=rParentRealRect.x+rParentRealRect.w*m_rRect.x;
		m_rRealRect.y=rParentRealRect.y+rParentRealRect.h*m_rRect.y;
		m_rRealRect.w=rParentRealRect.w*m_rRect.w;
		m_rRealRect.h=rParentRealRect.h*m_rRect.h;
	}
	else
	{
		m_rRealRect.x=rParentRealRect.x+m_rRect.x;
		m_rRealRect.y=rParentRealRect.y+m_rRect.y;
		m_rRealRect.w=m_rRect.w;
		m_rRealRect.h=m_rRect.h;
	}

	if(m_bCentered)
	{
		SGameRect rTempRect;
		rTempRect.w=rParentRealRect.w;
		rTempRect.h=rParentRealRect.h;
		m_rRealRect.CenterOnRect(&rTempRect);
	}

	for(unsigned x=0;x<m_vChildren.size();x++)
	{
		m_vChildren[x]->UpdateRealRect();
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

	for(i=m_vChildren.begin();i!=m_vChildren.end();i++)
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

	for(i=m_vChildren.begin();i!=m_vChildren.end();i++)
	{
		if(*i==piWindow)
		{
			m_vChildren.erase(i);
			m_vChildren.push_back(piWindow);
			break;
		}
	}
}

void CGameWindowBase::EnumerateChildren(IGameWindowEnumerationCallback *piCallback)
{
	bool bStopEnumerating=false;
	std::vector<IGameWindow*>::iterator i;

	for(i=m_vChildren.begin();i!=m_vChildren.end() && !bStopEnumerating;i++)
	{
		piCallback->ProcessEnumeratedWindow(*i,&bStopEnumerating);
	}
}

void CGameWindowBase::OnCharacter(int nCharacter,bool *pbProcessed)
{
}
void CGameWindowBase::OnKeyDown(int nKey,bool *pbProcessed)
{
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
		piRender->ActivateBlending();
		piRender->SetColor(CVector(1,1,1),1);
		piRender->SelectTexture(m_MouseCursorTexture.m_piTexture,0);
		piRender->RenderTextureRect(position.x,position.y-dHeight,dWidth,dHeight,0,0,1,1);
		piRender->UnselectTexture(0);
		piRender->DeactivateBlending();
		(*pbDrawed)=true;
	}
}

void CGameWindowBase::OnDrawBackground(IGenericRender *piRender)
{
	if(m_BackgroundTexture.m_piTexture)
	{
		piRender->ActivateBlending();
		piRender->SetColor(m_vBackgroundColor,m_dBackgroundAlpha);
		piRender->SelectTexture(m_BackgroundTexture.m_piTexture,0);
		piRender->RenderTexture(CVector(m_rRealRect.w*0.5,m_rRealRect.h*0.5,0),m_rRealRect.w,m_rRealRect.h);
		piRender->UnselectTexture(0);
		piRender->DeactivateBlending();
	}
	else
	{
		if(m_dBackgroundAlpha!=0.0)
		{
			piRender->Clear(m_vBackgroundColor,m_dBackgroundAlpha);
		}
	}

	if(m_BackgroundModel.m_piModel)
	{
		CVector vMaxs,vMins;
		m_BackgroundModel.m_piModel->GetFrameBBox(0,0,&vMins,&vMaxs);
		CVector vSize=vMaxs-vMins;
		CVector vCenter=(vMaxs+vMins)*0.5;
		vCenter.c[1]=vMaxs.c[1]+1;
		piRender->SetOrthographicProjection(vSize.c[0],vSize.c[2]);
		piRender->SetCamera(vCenter,0,-90,0);
		piRender->RenderModel(Origin,Origin,m_BackgroundModel.m_piModel);
	}
}
void CGameWindowBase::OnDraw(IGenericRender *piRender){}

void CGameWindowBase::OnSetFocus(){}
void CGameWindowBase::OnKillFocus(IGameWindow *piFocusedWindow){}
void CGameWindowBase::OnReleaseMouseCapture(){}

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

bool CGameWindowBase::ConfirmDialog(std::string sText,std::string sTitle,EMessageDialogType eType)
{	
	CConfirmDialogWrapper dialog;
	dialog.Attach("GameGUI","ConfirmDialog");
	bool bOk=false;
	if(dialog.m_piConfirmDialog){bOk=dialog.m_piConfirmDialog->Confirm(this,sText,sTitle,eType);}
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
