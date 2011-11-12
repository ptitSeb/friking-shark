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
#include "GameGUIColorDialog.h"

CGameGUIColorDialog::CGameGUIColorDialog(void)
{
	m_bDraggingH=false;
	m_bDraggingS=false;
	m_bDraggingV=false;
	m_bDraggingSV=false;
	m_piCallback=NULL;

	InitializeChildren();
}

CGameGUIColorDialog::~CGameGUIColorDialog(void)
{
}

void CGameGUIColorDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
}

void CGameGUIColorDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIColorDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTSelect){EndDialog(DIALOG_OK);}
	if(piControl==m_piBTCancel){EndDialog(DIALOG_CANCEL);}
}

void CGameGUIColorDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
	if(nKey==GK_RETURN){EndDialog(DIALOG_OK);*pbProcessed=true;}
}

bool CGameGUIColorDialog::SelectColor(IGameWindow *piParent,std::string sTitle,CVector *pvColor)
{	
	m_piCallback=NULL;
	m_vOriginalColor=*pvColor;
	m_vColor=RGBToHSV(m_vOriginalColor);
	m_sTitle=sTitle;
	if(Execute(piParent)!=DIALOG_OK)
	{
		return false;
	}
	*pvColor=HSVToRGB(m_vColor);
	return true;
}

bool CGameGUIColorDialog::SelectColor(IGameWindow *piParent,IGameGUIColorDialogCallback *piCallback,std::string sTitle,CVector *pvColor)
{	
	m_piCallback=piCallback;
	m_vOriginalColor=*pvColor;
	m_vColor=RGBToHSV(m_vOriginalColor);
	m_sTitle=sTitle;
	if(Execute(piParent)!=DIALOG_OK)
	{
		if(m_piCallback){m_piCallback->OnColorChanged(m_vOriginalColor);}
		return false;
	}
	*pvColor=HSVToRGB(m_vColor);
	if(m_piCallback){m_piCallback->OnColorChanged(*pvColor);}
	return true;
}

void CGameGUIColorDialog::OnMouseDown(int nButton,double x,double y)
{
	if(m_piSTSVSelector==NULL || m_piSTHSelector==NULL){return;}
	if(m_piSTSSelector==NULL || m_piSTVSelector==NULL){return;}
	
	m_piGUIManager->SetMouseCapture(this);
	
	SGameRect sSVRect,sHRect,sSRect,sVRect;
	m_piSTSVSelector->GetRealRect(&sSVRect);
	m_piSTHSelector->GetRealRect(&sHRect);
	m_piSTSSelector->GetRealRect(&sSRect);
	m_piSTVSelector->GetRealRect(&sVRect);
	
	sHRect.x-=m_rRealRect.x;
	sHRect.y-=m_rRealRect.y;
	sSRect.x-=m_rRealRect.x;
	sSRect.y-=m_rRealRect.y;
	sVRect.x-=m_rRealRect.x;
	sVRect.y-=m_rRealRect.y;
	sSVRect.x-=m_rRealRect.x;
	sSVRect.y-=m_rRealRect.y;
	
	SGamePos pos(x,y);
	if(sHRect.Contains(pos))
	{
		m_vColor.c[0]=((pos.y-sHRect.y)/sHRect.h)*360.0;
		if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
		m_bDraggingH=true;
	}
	if(sSRect.Contains(pos))
	{
		m_vColor.c[1]=((pos.y-sSRect.y)/sSRect.h);
		if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
		m_bDraggingS=true;
	}
	if(sVRect.Contains(pos))
	{
		m_vColor.c[2]=((pos.y-sVRect.y)/sVRect.h);
		if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
		m_bDraggingV=true;
	}	
	if(sSVRect.Contains(pos))
	{
		m_vColor.c[1]=((pos.y-sSVRect.y)/sSVRect.h);
		m_vColor.c[2]=((pos.x-sSVRect.x)/sSVRect.w);
		if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
		m_bDraggingSV=true;
	}
}

void CGameGUIColorDialog::OnMouseMove(double x,double y)
{
	if(m_piGUIManager->HasMouseCapture(this))
	{
	  SGameRect sSVRect,sHRect,sSRect,sVRect;
	  m_piSTSVSelector->GetRealRect(&sSVRect);
	  m_piSTHSelector->GetRealRect(&sHRect);
	  m_piSTSSelector->GetRealRect(&sSRect);
	  m_piSTVSelector->GetRealRect(&sVRect);
	  
	  sHRect.x-=m_rRealRect.x;
	  sHRect.y-=m_rRealRect.y;
	  sSRect.x-=m_rRealRect.x;
	  sSRect.y-=m_rRealRect.y;
	  sVRect.x-=m_rRealRect.x;
	  sVRect.y-=m_rRealRect.y;
	  sSVRect.x-=m_rRealRect.x;
	  sSVRect.y-=m_rRealRect.y;
	  
	  SGamePos pos(x,y);
	  if(m_bDraggingH && sHRect.Contains(pos))
	  {
		  m_vColor.c[0]=((pos.y-sHRect.y)/sHRect.h)*360.0;
		  if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
	  }
	  if(m_bDraggingS && sSRect.Contains(pos))
	  {
		  m_vColor.c[1]=((pos.y-sSRect.y)/sSRect.h);
		  if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
	  }
	  if(m_bDraggingV && sVRect.Contains(pos))
	  {
		  m_vColor.c[2]=((pos.y-sVRect.y)/sVRect.h);
		  if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
	  }
	  if(m_bDraggingSV && sSVRect.Contains(pos))
	  {
		  m_vColor.c[1]=((pos.y-sSVRect.y)/sSVRect.h);
		  m_vColor.c[2]=((pos.x-sSVRect.x)/sSVRect.w);
		  if(m_piCallback){m_piCallback->OnColorChanged(HSVToRGB(m_vColor));}
	   } 
	}
}

void CGameGUIColorDialog::OnMouseUp(int nButton,double x,double y)
{
	if(m_piGUIManager->HasMouseCapture(this)){m_piGUIManager->ReleaseMouseCapture();m_bDraggingH=false;m_bDraggingS=false;m_bDraggingV=false;m_bDraggingSV=false;}
}

void CGameGUIColorDialog::OnDraw(IGenericRender *piRender)
{
	CGameDialogBase::OnDraw(piRender);
	
	if(m_piSTSVSelector==NULL || m_piSTHSelector==NULL){return;}
	if(m_piSTSSelector==NULL || m_piSTVSelector==NULL){return;}
	
	CVector hsvColor=m_vColor;
	
	SGameRect sSVRect,sHRect,sSRect,sVRect;
	m_piSTSVSelector->GetRealRect(&sSVRect);
	m_piSTHSelector->GetRealRect(&sHRect);
	m_piSTSSelector->GetRealRect(&sSRect);
	m_piSTVSelector->GetRealRect(&sVRect);
	
	CVector vSVOrig(sSVRect.x,sSVRect.y,0);
	CVector vHOrig(sHRect.x,sHRect.y,0);
	CVector vSOrig(sSRect.x,sSRect.y,0);
	CVector vVOrig(sVRect.x,sVRect.y,0);
	
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
	
	vHeight=CVector(0,sSRect.h/100.0,0);
	for(double x=0;x<1.0;x+=0.01)
	{
	  vColors[0]=vColors[1]=vColors[2]=vColors[3]=HSVToRGB(CVector(hsvColor.c[0],x,hsvColor.c[2]));
	  vVertexes[0]=vSOrig;
	  vVertexes[1]=vSOrig+(AxisPosX*sSRect.w);
	  vVertexes[2]=vSOrig+(AxisPosX*sSRect.w)+vHeight;
	  vVertexes[3]=vSOrig+vHeight;
	  piRender->RenderPolygon(4,vVertexes,vColors);
	  vSOrig+=vHeight;
	}
	
	vHeight=CVector(0,sVRect.h/100.0,0);
	for(double x=0;x<1.0;x+=0.01)
	{
	  vColors[0]=vColors[1]=vColors[2]=vColors[3]=HSVToRGB(CVector(hsvColor.c[0],hsvColor.c[1],x));
	  vVertexes[0]=vVOrig;
	  vVertexes[1]=vVOrig+(AxisPosX*sVRect.w);
	  vVertexes[2]=vVOrig+(AxisPosX*sVRect.w)+vHeight;
	  vVertexes[3]=vVOrig+vHeight;
	  piRender->RenderPolygon(4,vVertexes,vColors);
	  vVOrig+=vHeight;
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
	CVector vWhite(1,1,1);
	CVector vBlack(0,0,0);
	// SV helper thumb (a circle)
	CVector vTemp=CVector(sSVRect.x,sSVRect.y,0)+CVector(sSVRect.w*hsvColor.c[2],sSVRect.h*hsvColor.c[1],0);
	piRender->RenderPoint(vTemp,5,vWhite,1);
	piRender->RenderPoint(vTemp,3,vBlack,1);

	// H helper thumb (a Line)
	piRender->SetColor(vWhite,1);
	piRender->RenderRect(sHRect.x,sHRect.y+((sHRect.h*hsvColor.c[0])/360.0)-1.0,sHRect.w,3);	
	piRender->SetColor(vBlack,1);
	piRender->RenderRect(sHRect.x,sHRect.y+(sHRect.h*hsvColor.c[0])/360.0,sHRect.w,1);	
	
	// S helper thumb (a Line)
	piRender->SetColor(vWhite,1);
	piRender->RenderRect(sSRect.x,sSRect.y+(sSRect.h*hsvColor.c[1])-1.0,sSRect.w,3);	
	piRender->SetColor(vBlack,1);
	piRender->RenderRect(sSRect.x,sSRect.y+(sSRect.h*hsvColor.c[1]),sSRect.w,1);	
		
	// V helper thumb (a Line)
	piRender->SetColor(vWhite,1);
	piRender->RenderRect(sVRect.x,sVRect.y+(sVRect.h*hsvColor.c[2])-1.0,sVRect.w,3);	
	piRender->SetColor(vBlack,1);
	piRender->RenderRect(sVRect.x,sVRect.y+(sVRect.h*hsvColor.c[2]),sVRect.w,1);	

	char sTemp[256];
	CVector vRGB=HSVToRGB(m_vColor);
	if(m_piSTSample){m_piSTSample->SetBackgroundColor(HSVToRGB(m_vColor),1);}	
	if(m_piSTRGB){sprintf(sTemp,"RGB: %.f,%.f,%.f",vRGB.c[0]*255.0,vRGB.c[1]*255.0,vRGB.c[2]*255.0);m_piSTRGB->SetText(sTemp);}	
	if(m_piSTHSV){sprintf(sTemp,"HSV: %.f,%.f,%.f",m_vColor.c[0],m_vColor.c[1]*100.0,m_vColor.c[2]*100.0);m_piSTHSV->SetText(sTemp);}	
}













