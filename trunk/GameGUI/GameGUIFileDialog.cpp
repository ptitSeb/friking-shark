#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIFileDialog.h"

CGameGUIFileDialog::CGameGUIFileDialog(void)
{
	m_piBTCancel=NULL;
	m_piBTSelect=NULL;
	m_piSTTitle=NULL;
	m_piEDPath=NULL;
	m_piLSFiles=NULL;
	m_bOverWriteWarn=true;
	m_bOpenMode=true;
}

CGameGUIFileDialog::~CGameGUIFileDialog(void)
{
}

void CGameGUIFileDialog::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
	if(m_piEDPath){m_piEDPath->SetText(m_sFile);}

	UpdateFiles();
}
void CGameGUIFileDialog::UpdateFiles()
{
	std::string sFile;
		
	if(m_piEDPath)	
	{
		char sTemp[MAX_PATH];
		sFile=m_piEDPath->GetText();
	}
	if(m_piLSFiles)
	{
		std::string sFilter=sFile+"*";
		std::set<std::string> sFiles;
		std::set<std::string>::iterator i;
		FindFiles(sFilter.c_str(),eFindFilesMode_DirsAndFiles,&sFiles);
		m_piLSFiles->Clear();
		for(i=sFiles.begin();i!=sFiles.end();i++)
		{
			m_piLSFiles->AddElement(*i);
		}
	}
}

void CGameGUIFileDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIFileDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTSelect){EndDialog(DIALOG_OK);}
	if(piControl==m_piBTCancel){EndDialog(DIALOG_CANCEL);}
}

void CGameGUIFileDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
	if(nKey==GK_RETURN){EndDialog(DIALOG_OK);*pbProcessed=true;}
	if(nKey=='\t'){UpdateFiles();*pbProcessed=true;}
}

bool CGameGUIFileDialog::OpenFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile)
{	
	m_sFile=*psFile;
	m_sTitle=sTitle;
	m_bOpenMode=true;
	if(Execute(piParent)!=DIALOG_OK){return false;}
	*psFile=m_sFile;
	return true;
}

bool CGameGUIFileDialog::SaveFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn)
{	
	m_sFile=*psFile;
	m_sTitle=sTitle;
	m_bOverWriteWarn=bOverWriteWarn;
	m_bOpenMode=false;
	if(Execute(piParent)!=DIALOG_OK){return false;}
	*psFile=m_sFile;
	return true;
}











