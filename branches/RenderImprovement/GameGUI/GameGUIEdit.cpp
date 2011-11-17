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
#include "GameGUIEdit.h"

#define EDITOR_BORDER_SIZE 1.0
#define EDITOR_TEXT_MARGIN 2.0
#define EDITOR_TOTAL_MARGIN (EDITOR_BORDER_SIZE +EDITOR_TEXT_MARGIN ) 
#define NON_SEPARATOR_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890_-"

bool IsSeparator(unsigned char c){return (strchr(NON_SEPARATOR_CHARACTERS,c)==NULL);}

CGameGUIEdit::CGameGUIEdit(void)
{
  	m_bActive=true;
	m_nEditionPos=0;
	m_nSelectionPos=m_nEditionPos;
	PersistencyDefaultValue();
}

CGameGUIEdit::~CGameGUIEdit(void)
{
}

void CGameGUIEdit::SetText(std::string sText)
{
	m_nEditionPos=0;
	m_nSelectionPos=m_nEditionPos;
	CGameGUILabel::SetText(sText);
}

void CGameGUIEdit::GetBorderColor(CVector *pvColor,double *pdAlpha)
{
	*pvColor=m_vBorderColor;
	*pdAlpha=m_dBorderAlpha;
}

void CGameGUIEdit::SetBorderColor(CVector vColor,double dAlpha)
{
	m_vBorderColor=vColor;
	m_dBorderAlpha=dAlpha;
}

void CGameGUIEdit::SetCursor(int nIndex)
{
	if(nIndex>(int)m_sText.length()){nIndex=m_sText.length()-1;}
	if(nIndex<0){nIndex=0;}
	m_nEditionPos=m_nSelectionPos=nIndex;
}


void CGameGUIEdit::DrawText(IGenericRender *piRender,CVector &vColor,double dAlpha)
{
	if(m_sText.length())
	{
		std::string sFontName;
		double			dFontSize=0;

		IGenericFont *piFont=NULL;
		GetFont(&piFont,&dFontSize);
		if(piFont)
		{
			if(m_bAutoResizeFont)
			{
				dFontSize=m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0;
			}
			piFont->RenderTextEx(piRender,dFontSize,EDITOR_TOTAL_MARGIN,EDITOR_TOTAL_MARGIN,m_rRealRect.w-EDITOR_TOTAL_MARGIN*2.0,m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0,m_sText.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment,vColor,dAlpha);
		}
		REL(piFont);
	}
}

void CGameGUIEdit::OnDraw(IGenericRender *piRender)
{
	piRender->PushState();
	double dTextWidth=0,dTextHeight=0;
	if(m_sText.length())
	{
		std::string sFontName;
		double dFontSize=0;

		IGenericFont *piFont=NULL;
		GetFont(&piFont,&dFontSize);
		if(piFont)
		{
			if(m_bAutoResizeFont)
			{
				dFontSize=m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0;
			}
			
			if(m_nSelectionPos!=m_nEditionPos)
			{
			  int nSelectionStart=std::min(m_nEditionPos,m_nSelectionPos);
			  int nSelectionEnd=std::max(m_nEditionPos,m_nSelectionPos);
			  std::string sPre=m_sText.substr(0,nSelectionStart);
			  std::string sSel=m_sText.substr(nSelectionStart,nSelectionEnd-nSelectionStart);
			  std::string sPost=m_sText.substr(nSelectionEnd,m_sText.length()-nSelectionEnd);
			  
			  double dx=m_rRealRect.x+EDITOR_TOTAL_MARGIN;
			  double dy=m_rRealRect.y+EDITOR_TOTAL_MARGIN;
			  double dw=m_rRealRect.w-EDITOR_TOTAL_MARGIN*2.0;
			  double dh=m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0;
			  if(sPre.length())
			  {
				piFont->CalcTextSize(dFontSize,sPre.c_str(),&dTextWidth,&dTextHeight);
				piFont->RenderTextEx(piRender,dFontSize,dx,dy,dw,dh,sPre.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment,m_vTextColor,m_dTextAlpha);
				dx+=dTextWidth;
				dw-=dTextWidth;
			  }
			  if(sSel.length())
			  {
				piFont->CalcTextSize(dFontSize,sSel.c_str(),&dTextWidth,&dTextHeight);
				piRender->RenderRect(dx,dy,std::min(dTextWidth,dw),dh,m_vSelectedBackgroundColor,m_dTextAlpha);
				piFont->RenderTextEx(piRender,dFontSize,dx,dy,dw,dh,sSel.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment,m_vSelectedTextColor,m_dTextAlpha);
				dx+=dTextWidth;
				dw-=dTextWidth;
			  }
			  if(sPost.length())
			  {
				piFont->RenderTextEx(piRender,dFontSize,dx,dy,dw,dh,sPost.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment,m_vTextColor,m_dTextAlpha);
			  }
			}
			else
			{
				piFont->RenderTextEx(piRender,dFontSize,m_rRealRect.x+EDITOR_TOTAL_MARGIN,m_rRealRect.y+EDITOR_TOTAL_MARGIN,m_rRealRect.w-EDITOR_TOTAL_MARGIN*2.0,m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0,m_sText.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment,m_vTextColor,m_dTextAlpha);
			}
			piFont->CalcTextSize(dFontSize,m_sText.substr(0,m_nEditionPos).c_str(),&dTextWidth,&dTextHeight);
		}
		REL(piFont);
	}
	bool bFocused=m_piGUIManager->HasFocus(this);
	piRender->ActivateBlending();
	piRender->DeactivateSolid();
	piRender->RenderRect(CVector(m_rRealRect.x+m_rRealRect.w*0.5,m_rRealRect.y+m_rRealRect.h*0.5,0),AxisNegX,AxisPosY,m_rRealRect.w-2.0,m_rRealRect.h-2.0,m_vBorderColor,bFocused?m_dBorderAlpha:m_dBorderAlpha*0.5);
	piRender->DeactivateBlending();
	
	if(bFocused)
	{
		piRender->RenderLine(CVector(m_rRealRect.x+dTextWidth+EDITOR_TOTAL_MARGIN,m_rRealRect.y+EDITOR_TOTAL_MARGIN,0),CVector(m_rRealRect.x+dTextWidth+EDITOR_TOTAL_MARGIN,m_rRealRect.y+m_rRealRect.h-EDITOR_TOTAL_MARGIN,0),0xFFFF,m_vTextColor,1);
	}
	piRender->PopState();

}

void CGameGUIEdit::OnCharacter( int nKey,bool *pbProcessed )
{
	DeleteSelection();

	unsigned long nCommand=0;
	char sCommand[10*1024];
	strcpy(sCommand,m_sText.c_str());
	nCommand=m_sText.length();

	if(nCommand<(sizeof(sCommand)-1))
	{
		memmove(sCommand+m_nEditionPos+1,sCommand+m_nEditionPos,nCommand-m_nEditionPos);
		sCommand[m_nEditionPos]=nKey;
		m_nEditionPos++;
		nCommand++;
		sCommand[nCommand]=0;
	}

	m_nSelectionPos=m_nEditionPos;
	m_sText=sCommand;
	*pbProcessed=true;
	
	NOTIFY_EVENT(IGameGUIEditEvents,OnTextChanged(this,m_sText))
}

void CGameGUIEdit::OnKeyDown( int nKey,bool *pbProcessed )
{
	bool bNotify=false;
	 
	int nCommand=0;
	char sCommand[10*1024];
	strcpy(sCommand,m_sText.c_str());
	nCommand=m_sText.length();

	if(nKey==GK_LEFT)
	{
		if(m_piGUIManager->IsKeyDown(GK_LCONTROL))
		{
		  int nOldPos=m_nEditionPos?m_nEditionPos-1:m_nEditionPos;
		  while(nOldPos>=0 && IsSeparator(sCommand[nOldPos])){m_nEditionPos=nOldPos;nOldPos--;}
		  while(nOldPos>=0 && !IsSeparator(sCommand[nOldPos])){m_nEditionPos=nOldPos;nOldPos--;}
		  m_nEditionPos=(nOldPos>=0)?m_nEditionPos:0;
		}
		else
		{ 
		  if(m_nEditionPos){m_nEditionPos--;}
		}
		if(!m_piGUIManager->IsKeyDown(GK_LSHIFT)){m_nSelectionPos=m_nEditionPos;}
		*pbProcessed=true;
	}
	else if(nKey==GK_RIGHT)
	{
		if(m_piGUIManager->IsKeyDown(GK_LCONTROL))
		{
		  int nOldPos=m_nEditionPos;
		  m_nEditionPos=nCommand;
		  while(nOldPos<nCommand && IsSeparator(sCommand[nOldPos])){m_nEditionPos=nOldPos;nOldPos++;}
		  while(nOldPos<nCommand && !IsSeparator(sCommand[nOldPos])){m_nEditionPos=nOldPos;nOldPos++;}
		  m_nEditionPos=nOldPos;
		}
		else
		{
		  if(m_nEditionPos<nCommand){m_nEditionPos++;}
		}
		if(!m_piGUIManager->IsKeyDown(GK_LSHIFT)){m_nSelectionPos=m_nEditionPos;}
		*pbProcessed=true;

	}
	else if(nKey==GK_HOME)
	{
		m_nEditionPos=0;
		if(!m_piGUIManager->IsKeyDown(GK_LSHIFT)){m_nSelectionPos=m_nEditionPos;}
		*pbProcessed=true;

	}
	else if(nKey==GK_END)
	{
		m_nEditionPos=nCommand;
		if(!m_piGUIManager->IsKeyDown(GK_LSHIFT)){m_nSelectionPos=m_nEditionPos;}
		*pbProcessed=true;

	}			
	else if(nKey==GK_BACK)
	{
		if(m_nEditionPos!=m_nSelectionPos)
		{
		  DeleteSelection();
		  return;
		}
		else if(m_nEditionPos)
		{
			memmove(sCommand+m_nEditionPos-1,sCommand+m_nEditionPos,nCommand-m_nEditionPos);
			nCommand--;
			m_nEditionPos--;
			m_nSelectionPos=m_nEditionPos;
			sCommand[nCommand]=0;
			bNotify=true;
		}
		*pbProcessed=true;
	}	
	else if(nKey==GK_DELETE)
	{
		if(m_nEditionPos!=m_nSelectionPos)
		{
		  DeleteSelection();
		  return;
		}
		else if(m_nEditionPos<nCommand)
		{
			memmove(sCommand+m_nEditionPos,sCommand+m_nEditionPos+1,nCommand-m_nEditionPos-1);
			nCommand--;
			sCommand[nCommand]=0;
			bNotify=true;
		}
		m_nSelectionPos=m_nEditionPos;
		*pbProcessed=true;
	}
	m_sText=sCommand;
	if(bNotify){NOTIFY_EVENT(IGameGUIEditEvents,OnTextChanged(this,m_sText));}
}

void CGameGUIEdit::DeleteSelection()
{
   if(m_nEditionPos!=m_nSelectionPos)
   {
	  int nSelectionStart=std::min(m_nEditionPos,m_nSelectionPos);
	  int nSelectionEnd=std::max(m_nEditionPos,m_nSelectionPos);
	  
  	 m_sText.replace(nSelectionStart,nSelectionEnd-nSelectionStart,"");	 
	 m_nEditionPos=m_nSelectionPos=nSelectionStart;

	 NOTIFY_EVENT(IGameGUIEditEvents,OnTextChanged(this,m_sText))
   }
}


int CGameGUIEdit::GetCharacterFromCoordinates(double x,double y)
{
	int nPos=m_sText.length();
	
	double dFontSize=0;
	IGenericFont *piFont=NULL;
	GetFont(&piFont,&dFontSize);
	if(piFont)
	{
		if(m_bAutoResizeFont)
		{
			dFontSize=m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0;
		}
		
		for(int c=0;c<nPos;c++)
		{
		  double w,h;
		  piFont->CalcTextSize(dFontSize,m_sText.substr(0,c).c_str(),&w,&h);
		  if((EDITOR_TOTAL_MARGIN+w)>x){return c?c-1:0;}
		}
	}
	REL(piFont);
	return nPos;
}

void CGameGUIEdit::OnMouseMove(double x,double y)
{
  CGameGUILabel::OnMouseMove(x,y);

  if(m_piGUIManager->HasMouseCapture(this))
  {
	m_nEditionPos=GetCharacterFromCoordinates(x,y);
  }
}

void CGameGUIEdit::OnMouseDown(int nButton,double x,double y)
{
  CGameGUILabel::OnMouseDown(nButton,x,y);

  int nPos=GetCharacterFromCoordinates(x,y);
  if(DetectDrag(x,y))
  {
	m_piGUIManager->SetMouseCapture(this);
	m_nEditionPos=nPos;
	m_nSelectionPos=m_nEditionPos;
  }
  else
  {  
	m_nEditionPos=nPos;
	if(!m_piGUIManager->IsKeyDown(GK_LSHIFT)){m_nSelectionPos=m_nEditionPos;}
  }
}

void CGameGUIEdit::OnMouseUp(int nButton,double x,double y)
{
  CGameGUILabel::OnMouseUp(nButton,x,y);
  
  if(m_piGUIManager->HasMouseCapture(this)){m_piGUIManager->ReleaseMouseCapture();}
}

void CGameGUIEdit::OnWantFocus(bool *pbWant){*pbWant=true;}

void CGameGUIEdit::GetSelection(int *pnFirst,int *pnLast)
{
	*pnFirst=std::min(m_nEditionPos,m_nSelectionPos);
	*pnLast=std::max(m_nEditionPos,m_nSelectionPos);
}

void CGameGUIEdit::SetSelection(int nFirst,int nLast)
{
	m_nEditionPos=std::max(0,std::min(nFirst,(int)m_sText.length()));
	m_nSelectionPos=std::max(0,std::min(nLast,(int)m_sText.length()));
}





