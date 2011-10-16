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
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GameGUI.h"
#include "GUISystems.h"
#include "HighScoresDialog.h"

CHighScoresDialog::CHighScoresDialog(void)
{
	m_bFromMainMenu=false;
	m_nScoreToEdit=-1;
	m_nFirstScoreVisible=0;
	m_nStartTime=0;
	m_bAlreadyFinished=false;
	m_piHighScoresMusicSound=NULL;
	m_piTable=NULL;
	m_nEditConfirmBlinkStart=0;
	m_nEditCommittedStart=0;
}

CHighScoresDialog::~CHighScoresDialog(void)
{
}

void CHighScoresDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	
	m_nEditConfirmBlinkStart=0;
	m_nEditCommittedStart=0;
	
	if(m_piTitle)
	{
		m_piTitle->SetText(m_nScoreToEdit!=-1?"Reserve Your Name !":"High Scores");
	}
	if(m_piBTOk)
	{
		if(m_bFromMainMenu)
		{
			m_piBTOk->SetText("Back");
		}
		else
		{
			m_piBTOk->SetText("Done");
		}
	}
	if(m_piEDName)
	{
		m_piEDName->Show(m_nScoreToEdit!=-1);
		
		if(m_nScoreToEdit!=-1 && m_piTable)
		{
			SHighScoreRow row=m_piTable->GetRow(m_nScoreToEdit);
			m_piEDName->SetText(row.sName);
			m_piEDName->SetSelection(0,row.sName.length());
			m_piGUIManager->SetFocus(m_piEDName);
		}
		else
		{
			m_piBTOk->DisableSounds();
			m_piGUIManager->SetFocus(m_piBTOk);
			m_piBTOk->EnableSounds();
		}
	}
}

bool CHighScoresDialog::Unserialize(ISystemPersistencyNode *piNode)
{
	bool bOk=CGameDialogBase::Unserialize(piNode);
	if(bOk)
	{
		if(m_HighScoresMusic.m_piSoundType)
		{
			m_piHighScoresMusicSound=m_HighScoresMusic.m_piSoundType->CreateInstance();
			if(m_piHighScoresMusicSound){m_piHighScoresMusicSound->SetLoop(false);}
		}
	}
	return bOk;
}

void CHighScoresDialog::Destroy()
{
	if(m_piHighScoresMusicSound)
	{
		if(m_piHighScoresMusicSound->IsPlaying()){m_piHighScoresMusicSound->Stop();}
		delete m_piHighScoresMusicSound;
		m_piHighScoresMusicSound=NULL;
	}
	CGameDialogBase::Destroy();
}


void CHighScoresDialog::ShowScores(IGameWindow *piParent,IHighScoresTable *piTable, int nScoreToEdit, bool bFromMainMenu)
{
	m_piTable=ADD(piTable);
	m_bFromMainMenu=bFromMainMenu;
	m_nScoreToEdit=nScoreToEdit;
	m_nFirstScoreVisible=(m_nScoreToEdit-(int)m_nRowCount+1);
	if(m_nFirstScoreVisible<0){m_nFirstScoreVisible=0;}
	
	Execute(piParent);
	m_nScoreToEdit=-1;
	REL(m_piTable);
}

int	CHighScoresDialog::Execute(IGameWindow *piParent)
{
	m_nStartTime=GetTimeStamp();
	m_bAlreadyFinished=false;
	if(!m_bFromMainMenu && m_piHighScoresMusicSound){m_piHighScoresMusicSound->Play();}
	int nRes=CGameDialogBase::Execute(piParent);
	if(!m_bFromMainMenu && m_piHighScoresMusicSound){m_piHighScoresMusicSound->Stop();}
	return nRes;
}

void CHighScoresDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){m_piBTOk->PlayClickSound();}
	
	if(m_nEditConfirmBlinkStart==0 &&
		m_nEditCommittedStart==0 && 
		m_nScoreToEdit!=-1 &&
		(nKey==GK_ESCAPE || nKey==GK_RETURN))
	{
		if(m_piEDName){m_piEDName->Show(false);}
		m_nEditConfirmBlinkStart=GetTimeStamp();
		*pbProcessed=true;
		return;
	}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}

void CHighScoresDialog::OnButtonClicked(IGameGUIButton* piControl)
{
	if(piControl==m_piBTOk)
	{
		if(m_nEditConfirmBlinkStart==0 &&
			m_nEditCommittedStart==0 && 
			m_nScoreToEdit!=-1)
		{
			if(m_piEDName){m_piEDName->Show(false);}
			m_nEditConfirmBlinkStart=GetTimeStamp();
			return;
		}
		
		EndDialog(DIALOG_OK);
	}
}

void CHighScoresDialog::OnDraw(IGenericRender *piRender)
{
	CGameDialogBase::OnDraw(piRender);
	
	if(m_nEditCommittedStart && (m_nEditCommittedStart+2000)<GetTimeStamp())
	{
		if(!m_bAlreadyFinished){m_bAlreadyFinished=true;EndDialog(0);}
	}
	else if(m_nEditConfirmBlinkStart && (m_nEditConfirmBlinkStart+1500)<GetTimeStamp())
	{
		m_nScoreToEdit=-1;
		m_nEditConfirmBlinkStart=0;
		m_nEditCommittedStart=GetTimeStamp();
	}
	else
	{
		if(!m_bFromMainMenu)
		{
			if(m_piHighScoresMusicSound==NULL)
			{
				if(!m_nEditCommittedStart && !m_nEditConfirmBlinkStart && (m_nStartTime+10*1000)<GetTimeStamp())
				{
					if(m_nScoreToEdit!=-1)
					{
						m_nEditConfirmBlinkStart=GetTimeStamp();
						if(m_piEDName){m_piEDName->Show(false);}	
					}
					else
					{
						EndDialog(DIALOG_OK);
					}
				}
					
			}
			else
			{
				if(!m_nEditCommittedStart && !m_nEditConfirmBlinkStart && !m_piHighScoresMusicSound->IsPlaying())
				{
					if(m_nScoreToEdit!=-1)
					{
						m_nEditConfirmBlinkStart=GetTimeStamp();
						if(m_piEDName){m_piEDName->Show(false);}	
					}
					else
					{
						EndDialog(DIALOG_OK);
					}
				}
			}
		}
	}
	
	if(m_piTable==NULL){return;}
		
	for(unsigned int x=0;x<eHighScoresColumn_Count;x++)
	{
		if(m_piGRColumns[x]==0){return;}
	}
	
	SGameRect 			 psGroupRects[eHighScoresColumn_Count];
	SGameRect 			 psRects[eHighScoresColumn_Count];
	IGenericFont 		*ppiFonts[eHighScoresColumn_Count]={0};
	double 				 pdFontSizes[eHighScoresColumn_Count]={0};
	
	for(unsigned int x=0;x<eHighScoresColumn_Count;x++)
	{
		m_piGRColumns[x]->GetFont(&ppiFonts[x],&pdFontSizes[x]);
		m_piGRColumns[x]->GetRealRect(&psGroupRects[x]);
		psRects[x]=psGroupRects[x];
		psRects[x].y+=psRects[x].h-m_nRowSize;
		psRects[x].h=m_nRowSize;
	}

	for(unsigned int x=0;x+m_nFirstScoreVisible<m_piTable->GetRowCount() && x<m_nRowCount;x++)
	{
		int nRow=x+m_nFirstScoreVisible;
		SHighScoreRow row=m_piTable->GetRow(nRow);
		
		char sRanking[100];
		char sDifficulty[100];
		char sScore[100];
		sprintf(sRanking,"%2d",nRow+1);
		sprintf(sScore,"%9d",row.nScore);
		
		if(nRow%10==0 && nRow!=10){strcat(sRanking,"st");}
		else if(nRow%10==1 && nRow!=11){strcat(sRanking,"nd");}
		else if(nRow%10==2 && nRow!=12){strcat(sRanking,"rd");}
		else {strcat(sRanking,"th");}
		
		switch(row.eDifficulty)
		{
			case eGameDifficulty_Easy:strcpy(sDifficulty,"Easy");break;
			case eGameDifficulty_Normal:strcpy(sDifficulty,"Normal");break;
			case eGameDifficulty_Hard:strcpy(sDifficulty,"Hard");break;
			case eGameDifficulty_VeryHard:strcpy(sDifficulty,"Very Hard");break;
			default:strcpy(sDifficulty,"Unknown");break;
		};
		
		eTextAlignment pAlignments[eHighScoresColumn_Count]={eTextAlignment_Center,eTextAlignment_Center,eTextAlignment_Center,eTextAlignment_Left};
		const char *ppTexts[eHighScoresColumn_Count]={sRanking,sScore,sDifficulty,row.sName.c_str()};
		for(unsigned int c=0;c<eHighScoresColumn_Count;c++)
		{
			bool bEditorCell=(m_nScoreToEdit==(int)nRow && m_piEDName && m_piEDName->IsVisible() && c==eHighScoresColumn_Name);
			if(bEditorCell)
			{
				SGameRect r;
				r.x=0;
				r.w=psGroupRects[c].w;
				r.h=m_nRowSize;
				r.y=psRects[c].y-psGroupRects[c].y;
				m_piEDName->SetRect(&r);
				
				int nFont=(GetTimeStamp()%250>125?1:0);
				if(m_nEditConfirmBlinkStart){nFont=(GetTimeStamp()%150>50?2:1);}
				double dSize=0;
				m_piEDName->GetFont(NULL,&dSize);
				m_piEDName->SetFont(m_EditionFonts[nFont].m_piFont,dSize);
			}
			else
			{				
				IGenericFont *piFont=ppiFonts[c];
				if(m_nScoreToEdit==(int)nRow)
				{
					bool bBlink=(c==eHighScoresColumn_Name || c==eHighScoresColumn_Ranking);
					int nFont=bBlink?(GetTimeStamp()%250>125?1:0):0;
					if(m_nEditConfirmBlinkStart){nFont=(GetTimeStamp()%150>50?2:0);}
					
					piFont=m_EditionFonts[nFont].m_piFont;
				}
				piFont->RenderTextEx(piRender,pdFontSizes[c],psRects[c].x,psRects[c].y,psRects[c].w,psRects[c].h,ppTexts[c],pAlignments[c],eTextAlignment_Center);
			}
			psRects[c].y-=m_nRowSize;
		}
	}
	for(unsigned int x=0;x<eHighScoresColumn_Count;x++)
	{
		REL(ppiFonts[x]);
	}
}

void CHighScoresDialog::OnTextChanged(IGameGUIEdit *piControl,std::string sNewText)
{
	if(m_piTable==NULL || m_nScoreToEdit==-1){return;}
	if(m_piEDName==NULL || !m_piEDName->IsVisible()){return;}
	
	SHighScoreRow sRow=m_piTable->GetRow(m_nScoreToEdit);
	sRow.sName=sNewText;
	m_piTable->SetRow(m_nScoreToEdit,sRow);
}

