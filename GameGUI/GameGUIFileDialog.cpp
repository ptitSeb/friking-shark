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
	if(m_sFile==""){m_sFile=AppendPathSeparator(GetWorkingFolder());}
	if(m_piSTTitle){m_piSTTitle->SetText(m_sTitle);}
	if(m_piEDPath)
	{
		m_piEDPath->SetText(m_sFile);
		m_piEDPath->SetCursor(m_sFile.length());
		m_piGUIManager->SetFocus(m_piEDPath);
	}
	UpdateFiles();
}

void CGameGUIFileDialog::UpdateFiles()
{
	std::string sFile;
	if(m_piEDPath){sFile=m_piEDPath->GetText();}
	if(m_piLSFiles)
	{
		std::string sFilter=sFile+"*";
		std::set<std::string> sFiles;
		std::set<std::string>::iterator i;
		m_piLSFiles->Clear();

		FindFiles((sFilter).c_str(),eFindFilesMode_OnlyDirs,&sFiles);
		for(i=sFiles.begin();i!=sFiles.end();i++){m_piLSFiles->AddElement(*i);}
		sFiles.clear();
		FindFiles((sFilter).c_str(),eFindFilesMode_OnlyFiles,&sFiles);
		for(i=sFiles.begin();i!=sFiles.end();i++)
		{
			if(i->length()<m_sPattern.length()){continue;}
			if(strcmp(i->c_str()+i->length()-m_sPattern.length(),m_sPattern.c_str())!=0){continue;}
			m_piLSFiles->AddElement(*i);
		}
	}
}

void CGameGUIFileDialog::AutoComplete()
{
	std::string sFile;
	if(m_piEDPath){sFile=m_piEDPath->GetText();}
	
	std::string sFilter=sFile+"*";
	std::set<std::string> sCandidateFiles;
	std::set<std::string> sFiles;
	std::set<std::string>::iterator i;
	FindFiles(sFilter.c_str(),eFindFilesMode_DirsAndFiles,&sFiles);
	for(i=sFiles.begin();i!=sFiles.end();i++)
	{
	  const char *pEditFile=sFile.c_str();
	  const char *pListFile=i->c_str();
	  if(strncmp(pListFile,pEditFile,sFile.length())!=0){continue;}
	  sCandidateFiles.insert(*i);
	}
	if(sCandidateFiles.size())
	{
	  while(true)
	  {
		int nPos=sFile.length();
		bool bFinished=true;
		unsigned char nChar=sCandidateFiles.begin()->c_str()[nPos];
		if(nChar==0){break;}
		for(i=sCandidateFiles.begin();i!=sCandidateFiles.end();i++)
		{
		  if(i->c_str()[nPos]!=nChar){bFinished=false;break;}
		}
		if(!bFinished){break;}
		sFile+=nChar;
	  }
	}
	if(m_piEDPath)
	{
	  std::string sNormalized=NormalizePath(sFile);
	  m_piEDPath->SetText(sNormalized);
	  m_piEDPath->SetCursor(sNormalized.length());
	  
	  UpdateFiles();
	}
}

void CGameGUIFileDialog::OnEndDialog()
{
	CGameDialogBase::OnEndDialog();
}

void CGameGUIFileDialog::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTSelect){ProcessSelect();}
	if(piControl==m_piBTCancel){EndDialog(DIALOG_CANCEL);}
}

void CGameGUIFileDialog::ProcessSelect()
{
	std::string sFile=m_sFile;
	if(m_piEDPath){sFile=m_piEDPath->GetText();}
	
	if(FileIsDirectory(sFile.c_str()))
	{
	  UpdateFiles();
	  return;
	}
	
	if(m_bOpenMode)
	{
	  if(!FileExists(sFile.c_str()))
	  {
		MessageDialog((std::string)"The file '"+sFile+"' does not exist.","Error",eMessageDialogType_Error);
		if(m_piEDPath){m_piGUIManager->SetFocus(m_piEDPath);}
		return;
	  }
	}
	else
	{
	  if(FileExists(sFile.c_str()) && m_bOverWriteWarn)
	  {
		if(ConfirmDialog((std::string)"Overwrite '"+sFile+"' ?.","Warning!",eMessageDialogType_Warning)!=DIALOG_OK)
		{
		  if(m_piEDPath){m_piGUIManager->SetFocus(m_piEDPath);}
		  return;
		}
	  }
	}
	m_sFile=sFile;
	EndDialog(DIALOG_OK);
}

void CGameGUIFileDialog::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_RETURN){ProcessSelect();*pbProcessed=true;}
	if(nKey==GK_ESCAPE){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
	if(nKey=='\t'){AutoComplete();*pbProcessed=true;}
}

bool CGameGUIFileDialog::OpenFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile)
{	
	m_sFile=*psFile;
	m_sTitle=sTitle;
	m_sPattern=psFilter;
	m_bOpenMode=true;
	if(Execute(piParent)!=DIALOG_OK){return false;}
	*psFile=m_sFile;
	return true;
}

bool CGameGUIFileDialog::SaveFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn)
{	
	m_sFile=*psFile;
	m_sTitle=sTitle;
	m_sPattern=psFilter;
	m_bOverWriteWarn=bOverWriteWarn;
	m_bOpenMode=false;
	if(Execute(piParent)!=DIALOG_OK){return false;}
	*psFile=m_sFile;
	return true;
}

void CGameGUIFileDialog::OnTextChanged(IGameGUIEdit *piControl,std::string sNewText)
{
  UpdateFiles();
}

void CGameGUIFileDialog::OnSelectionChanged(IGameGUIList *piControl,unsigned int nElement,std::string sElement)
{
  if(m_piEDPath)
  {
    std::string sNormalized=NormalizePath(sElement);
	m_piEDPath->SetText(sNormalized);
	m_piEDPath->SetCursor(sNormalized.length());
  }
}

void CGameGUIFileDialog::OnSelectionDoubleCliked(IGameGUIList *piControl,unsigned int nElement,std::string sElement)
{
  if(m_piEDPath)
  {
    std::string sNormalized=NormalizePath(sElement);
	m_piEDPath->SetText(sNormalized);
	m_piEDPath->SetCursor(sNormalized.length());
  }
  ProcessSelect();
}