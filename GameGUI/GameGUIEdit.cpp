#include "stdafx.h"
#include "GameGUILib.h"
#include "GameGUIEdit.h"

#define EDITOR_BORDER_SIZE 1.0
#define EDITOR_TEXT_MARGIN 2.0
#define EDITOR_TOTAL_MARGIN (EDITOR_BORDER_SIZE +EDITOR_TEXT_MARGIN ) 

CGameGUIEdit::CGameGUIEdit(void)
{
	m_nEditionPos=0;
	PersistencyDefaultValue();
}

CGameGUIEdit::~CGameGUIEdit(void)
{
}

void CGameGUIEdit::SetText(std::string sText)
{
	m_nEditionPos=0;
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
			piRender->SetColor(vColor,dAlpha);
			piFont->RenderTextEx(dFontSize,EDITOR_TOTAL_MARGIN,EDITOR_TOTAL_MARGIN,m_rRealRect.w-EDITOR_TOTAL_MARGIN*2.0,m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0,m_sText.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment);
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
		double			dFontSize=0;

		IGenericFont *piFont=NULL;
		GetFont(&piFont,&dFontSize);
		if(piFont)
		{
			if(m_bAutoResizeFont)
			{
				dFontSize=m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0;
			}
			piRender->SetColor(m_vTextColor,m_dTextAlpha);
			piFont->RenderTextEx(dFontSize,EDITOR_TOTAL_MARGIN,EDITOR_TOTAL_MARGIN,m_rRealRect.w-EDITOR_TOTAL_MARGIN*2.0,m_rRealRect.h-EDITOR_TOTAL_MARGIN*2.0,m_sText.c_str(),m_eHorizontalAlignment,m_eVerticalAlignment);
			piFont->CalcTextSize(dFontSize,m_sText.substr(0,m_nEditionPos).c_str(),&dTextWidth,&dTextHeight);
		}
		REL(piFont);
	}

	bool bFocused=(m_piGUIManager->GetFocus()==this);

	piRender->ActivateBlending();
	piRender->DeactivateSolid();
	piRender->SetColor(m_vBorderColor,bFocused?m_dBorderAlpha:m_dBorderAlpha*0.5);
	piRender->RenderRect(CVector(m_rRealRect.w*0.5,m_rRealRect.h*0.5,0),AxisNegX,AxisPosY,m_rRealRect.w-2.0,m_rRealRect.h-2.0);
	piRender->DeactivateBlending();
	if(bFocused)
	{
		piRender->RenderLine(CVector(dTextWidth+EDITOR_TOTAL_MARGIN,EDITOR_TOTAL_MARGIN,0),CVector(dTextWidth+EDITOR_TOTAL_MARGIN,m_rRealRect.h-EDITOR_TOTAL_MARGIN,0),m_vTextColor,0xFFFF);
	}
	piRender->PopState();

}

void CGameGUIEdit::OnCharacter( int nKey,bool *pbProcessed )
{
	if(nKey<32){return;}

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

	m_sText=sCommand;
	*pbProcessed=true;
}

void CGameGUIEdit::OnKeyDown( int nKey,bool *pbProcessed )
{
	int nCommand=0;
	char sCommand[10*1024];
	strcpy(sCommand,m_sText.c_str());
	nCommand=m_sText.length();

	if(nKey==VK_LEFT)
	{
		if(m_nEditionPos){m_nEditionPos--;}
		*pbProcessed=true;
	}
	else if(nKey==VK_RIGHT)
	{
		if(m_nEditionPos<nCommand){m_nEditionPos++;}
		*pbProcessed=true;

	}
	else if(nKey==VK_HOME)
	{
		m_nEditionPos=0;
		*pbProcessed=true;

	}
	else if(nKey==VK_END)
	{
		m_nEditionPos=nCommand;
		*pbProcessed=true;

	}			
	else if(nKey==VK_BACK)
	{
		if(m_nEditionPos)
		{
			memmove(sCommand+m_nEditionPos-1,sCommand+m_nEditionPos,nCommand-m_nEditionPos);
			nCommand--;
			m_nEditionPos--;
			sCommand[nCommand]=0;
		}
		*pbProcessed=true;
	}	
	else if(nKey==VK_DELETE)
	{
		if(m_nEditionPos<nCommand)
		{
			memmove(sCommand+m_nEditionPos,sCommand+m_nEditionPos+1,nCommand-m_nEditionPos-1);
			nCommand--;
			sCommand[nCommand]=0;
		}
		*pbProcessed=true;
	}
	m_sText=sCommand;
}