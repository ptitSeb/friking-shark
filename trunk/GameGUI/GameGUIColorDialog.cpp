#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIColorDialog.h"

CGameGUIColorDialog::CGameGUIColorDialog(void)
{
	m_piBTCancel=NULL;
	m_piBTSelect=NULL;
	m_piSTTitle=NULL;
	m_piSTRGB=NULL;
	m_piSTHSV=NULL;
	m_piSTHSelector=NULL;
	m_piSTSVSelector=NULL;
	m_bDraggingH=false;
	m_bDraggingSV=false;
}

CGameGUIColorDialog::~CGameGUIColorDialog(void)
{
}

void CGameGUIColorDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
	if(m_piSTHSelector){m_piSTHSelector->Activate(false);}
	if(m_piSTSVSelector){m_piSTSVSelector->Activate(false);}
}

void CGameGUIColorDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIColorDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTSelect){EndDialog(IDOK);}
	if(piControl==m_piBTCancel){EndDialog(IDCANCEL);}
}

void CGameGUIColorDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==VK_ESCAPE){EndDialog(IDCANCEL);*pbProcessed=true;}
	if(nKey==VK_RETURN){EndDialog(IDOK);*pbProcessed=true;}
}

bool CGameGUIColorDialog::SelectColor(IGameWindow *piParent,std::string sTitle,CVector *pvColor)
{	
	m_vColor=RGBToHSV(*pvColor);
	m_sTitle=sTitle;
	if(Execute(piParent)!=IDOK){return false;}
	*pvColor=HSVToRGB(m_vColor);
	return true;
}

void CGameGUIColorDialog::OnMouseDown(int nButton,double x,double y)
{
	if(m_piSTSVSelector==NULL || m_piSTHSelector==NULL){return;}
	m_piGUIManager->SetMouseCapture(this);
	
	SGameRect sSVRect;
	SGameRect sHRect;
	m_piSTSVSelector->GetRealRect(&sSVRect);
	m_piSTHSelector->GetRealRect(&sHRect);
	
	sHRect.x-=m_rRealRect.x;
	sHRect.y-=m_rRealRect.y;
	sSVRect.x-=m_rRealRect.x;
	sSVRect.y-=m_rRealRect.y;
	
	SGamePos pos(x,y);
	if(sHRect.Contains(pos))
	{
		m_vColor.c[0]=((pos.y-sHRect.y)/sHRect.h)*360.0;
		m_bDraggingH=true;
	}
	if(sSVRect.Contains(pos))
	{
		m_vColor.c[1]=((pos.y-sSVRect.y)/sSVRect.h);
		m_vColor.c[2]=((pos.x-sSVRect.x)/sSVRect.w);
		m_bDraggingSV=true;
	}
}

void CGameGUIColorDialog::OnMouseMove(double x,double y)
{
	IGameWindow *piCapture=m_piGUIManager->GetMouseCapture();
	if(piCapture==this)
	{
	  SGameRect sSVRect;
	  SGameRect sHRect;
	  m_piSTSVSelector->GetRealRect(&sSVRect);
	  m_piSTHSelector->GetRealRect(&sHRect);
	  
	  sHRect.x-=m_rRealRect.x;
	  sHRect.y-=m_rRealRect.y;
	  sSVRect.x-=m_rRealRect.x;
	  sSVRect.y-=m_rRealRect.y;
	  
	  SGamePos pos(x,y);
	  if(m_bDraggingH && sHRect.Contains(pos))
	  {
		  m_vColor.c[0]=((pos.y-sHRect.y)/sHRect.h)*360.0;
	  }
	  if(m_bDraggingSV && sSVRect.Contains(pos))
	  {
		  m_vColor.c[1]=((pos.y-sSVRect.y)/sSVRect.h);
		  m_vColor.c[2]=((pos.x-sSVRect.x)/sSVRect.w);
	  } 
	}
	REL(piCapture);
}

void CGameGUIColorDialog::OnMouseUp(int nButton,double x,double y)
{
	IGameWindow *piCapture=m_piGUIManager->GetMouseCapture();
	if(piCapture==this){m_piGUIManager->ReleaseMouseCapture();m_bDraggingH=false;m_bDraggingSV=false;}
	REL(piCapture);
}

void CGameGUIColorDialog::OnDraw(IGenericRender *piRender)
{
	CGameDialogBase::OnDraw(piRender);
	
	if(m_piSTSVSelector==NULL || m_piSTHSelector==NULL){return;}
	
	CVector hsvColor=m_vColor;
	
	SGameRect sSVRect;
	SGameRect sHRect;
	m_piSTSVSelector->GetRealRect(&sSVRect);
	m_piSTHSelector->GetRealRect(&sHRect);
	
	sHRect.x-=m_rRealRect.x;
	sHRect.y-=m_rRealRect.y;
	sSVRect.x-=m_rRealRect.x;
	sSVRect.y-=m_rRealRect.y;
	
	CVector vSVOrig(sSVRect.x,sSVRect.y,0);
	CVector vHOrig(sHRect.x,sHRect.y,0);
	
	CVector vVertexes[4];
	CVector vColors[4];
	CVector vHeight=CVector(0,sHRect.h/360.0,0);
	for(int x=0;x<360;x++)
	{
	  vColors[0]=vColors[1]=vColors[2]=vColors[3]=HSVToRGB(CVector(x,1,1));
	  vVertexes[0]=vHOrig;
	  vVertexes[1]=vHOrig+(AxisPosX*sHRect.w);
	  vVertexes[2]=vHOrig+(AxisPosX*sHRect.w)+vHeight;
	  vVertexes[3]=vHOrig+vHeight;
	  piRender->RenderPolygon(4,vVertexes,vColors);
	  vHOrig+=vHeight;
	}
	
	vHeight=CVector(0,sSVRect.h/50.0,0);
	CVector vWidth=CVector(sSVRect.w/50.0,0,0);
	for(int x=0;x<50;x++)
	{
	  CVector vTemp=vSVOrig;
	  for(int y=0;y<50;y++)
	  {
		vColors[0]=vColors[1]=vColors[2]=vColors[3]=HSVToRGB(CVector(hsvColor.c[0],((double)y)*0.02,((double)x)*0.02));
		vVertexes[0]=vSVOrig;
		vVertexes[1]=vSVOrig+vWidth;
		vVertexes[2]=vSVOrig+vWidth+vHeight;
		vVertexes[3]=vSVOrig+vHeight;
		piRender->RenderPolygon(4,vVertexes,vColors);
		vSVOrig+=vHeight;
	  }
	  vSVOrig=vTemp+vWidth;
	}
	
	char sTemp[256];
	CVector vRGB=HSVToRGB(m_vColor);
	if(m_piSTSample){m_piSTSample->SetBackgroundColor(HSVToRGB(m_vColor),1);}	
	if(m_piSTRGB){sprintf(sTemp,"RGB: %.f,%.f,%.f",vRGB.c[0]*255.0,vRGB.c[1]*255.0,vRGB.c[2]*255.0);m_piSTRGB->SetText(sTemp);}	
	if(m_piSTHSV){sprintf(sTemp,"HSV: %.f,%.f,%.f",m_vColor.c[0],m_vColor.c[1]*100.0,m_vColor.c[2]*100.0);m_piSTHSV->SetText(sTemp);}	
}













