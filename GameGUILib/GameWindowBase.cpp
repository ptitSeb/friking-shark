#include "StdAfx.h"
#include ".\gamewindowbase.h"
#include "commdlg.h"


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
		if(m_piGUIManager->GetFocus()==this)
		{
			m_piGUIManager->SetFocus(m_piParent);
		}
		IGameWindow *piCapture=m_piGUIManager->GetMouseCapture();
		if(piCapture==this)
		{
			m_piGUIManager->ReleaseMouseCapture();
		}
		REL(piCapture);
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
	if(m_Font.m_piFont==NULL)
	{
		m_piParent->GetFont(ppiFont,pdSize);
	}
	else
	{
		ADD(m_Font.m_piFont);
		(*ppiFont)=m_Font.m_piFont;
		(*pdSize)=m_dFontSize;
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
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);

	char sFileName[MAX_PATH	];
	strcpy(sFileName,psFile->c_str());
	OPENFILENAME ofn={0};
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=threadInfo.hwndActive;
	ofn.lpstrFilter=psFilter;
	ofn.lpstrFile=sFileName;
	ofn.nMaxFile=sizeof(sFileName);
	ofn.lpstrTitle=sTitle.c_str();
	ofn.Flags=OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR;
	ShowCursor(true);
	bool bOk=(GetOpenFileName(&ofn)==TRUE);
	if(bOk){*psFile=sFileName;}
	ShowCursor(false);
	return bOk;
}

bool CGameWindowBase::SaveFileDialog(std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn)
{
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);

	char sFileName[MAX_PATH	];
	strcpy(sFileName,psFile->c_str());
	OPENFILENAME ofn={0};
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=threadInfo.hwndActive;
	ofn.lpstrFilter=psFilter;
	ofn.lpstrFile=sFileName;
	ofn.nMaxFile=sizeof(sFileName);
	ofn.lpstrTitle=sTitle.c_str();
	ofn.Flags=OFN_PATHMUSTEXIST;
	if(bOverWriteWarn){ofn.Flags|=OFN_OVERWRITEPROMPT;}
	ShowCursor(true);
	bool bOk=(GetSaveFileName(&ofn)==TRUE);
	if(bOk){*psFile=sFileName;}
	ShowCursor(false);
	return bOk;
}

bool CGameWindowBase::SelectColorDialog(std::string sTitle,CVector *pvColor)
{
	COLORREF customcolors[16]={0};
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);

	CHOOSECOLOR colorInfo={0};
	colorInfo.lStructSize=sizeof(colorInfo);
	colorInfo.hwndOwner=threadInfo.hwndActive;
	colorInfo.rgbResult=VectorToRGB(pvColor);
	colorInfo.Flags=CC_ANYCOLOR|CC_RGBINIT|CC_FULLOPEN;
	colorInfo.lpCustColors=customcolors;
	ShowCursor(true);
	bool bOk=(ChooseColor(&colorInfo)==TRUE);
	ShowCursor(false);
	if(bOk)
	{
		*pvColor=RGBToVector(colorInfo.rgbResult);
	}
	return bOk;
}

bool CGameWindowBase::ConfirmDialog(std::string sText,std::string sTitle,EMessageDialogType eType)
{
	COLORREF customcolors[16]={0};
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);
	DWORD dwFlags=MB_YESNO;
	if(eType==eMessageDialogType_Info){dwFlags|=MB_ICONINFORMATION;}
	if(eType==eMessageDialogType_Warning){dwFlags|=MB_ICONEXCLAMATION;}
	if(eType==eMessageDialogType_Error){dwFlags|=MB_ICONERROR;}
	if(eType==eMessageDialogType_Question){dwFlags|=MB_ICONQUESTION;}
	ShowCursor(true);
	bool bYes=(MessageBox(threadInfo.hwndActive,sText.c_str(),sTitle.c_str(),dwFlags)==IDYES);
	ShowCursor(false);
	return bYes;
}

void CGameWindowBase::MessageDialog(std::string sText,std::string sTitle,EMessageDialogType eType)
{
	COLORREF customcolors[16]={0};
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);
	DWORD dwFlags=MB_OK;
	if(eType==eMessageDialogType_Info){dwFlags|=MB_ICONINFORMATION;}
	if(eType==eMessageDialogType_Warning){dwFlags|=MB_ICONEXCLAMATION;}
	if(eType==eMessageDialogType_Error){dwFlags|=MB_ICONERROR;}
	if(eType==eMessageDialogType_Question){dwFlags|=MB_ICONQUESTION;}

	ShowCursor(true);
	MessageBox(threadInfo.hwndActive,sText.c_str(),sTitle.c_str(),dwFlags);
	ShowCursor(false);
}

bool CGameWindowBase::DetectDrag(double dx,double dy)
{
	GUITHREADINFO threadInfo={0};
	threadInfo.cbSize=sizeof(threadInfo);
	GetGUIThreadInfo(GetCurrentThreadId(),&threadInfo);
	DWORD dwFlags=MB_OK;
	POINT pt;
	pt.x=(LONG)dx;
	pt.y=(LONG)((m_rRealRect.h-1)-dy);
	return (DragDetect(threadInfo.hwndActive,pt)?true:false);
}
