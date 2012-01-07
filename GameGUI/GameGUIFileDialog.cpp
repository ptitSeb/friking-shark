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
#include "GameGUIFileDialog.h"

CGameGUIFileDialog::CGameGUIFileDialog(void)
{
	m_bOverWriteWarn=true;
	m_bOpenMode=true;
	InitializeChildren();
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
			const char  *pFile=i->c_str();
			unsigned int nFile=i->length();
			if(m_vPatterns.size()==0)
			{
			  m_piLSFiles->AddElement(*i);
			}
			else
			{
			  for(unsigned int x=0;x<m_vPatterns.size();x++)
			  {
				unsigned int nPattern=m_vPatterns[x].length();
				const char  *pPattern=m_vPatterns[x].c_str();
				if(nFile>=nPattern &&
				  strcmp(pFile+nFile-nPattern,pPattern)==0)
				{
				  m_piLSFiles->AddElement(*i);
				  break;
				}
			  }
			}
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
	if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Accept,nKey)){ProcessSelect();*pbProcessed=true;}
	if(m_piGUIManager->IsNavigationControl(eGameGUINavigationControl_Cancel,nKey)){EndDialog(DIALOG_CANCEL);*pbProcessed=true;}
	if(nKey=='\t'){AutoComplete();*pbProcessed=true;}
}

bool CGameGUIFileDialog::OpenFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile)
{	
	m_sFile=*psFile;
	m_sTitle=sTitle;

	m_vPatterns.clear();
	char *pTemp=NULL;
	char *pPatternList=strdup(psFilter);
	char *pPattern=strtok_r(pPatternList,";",&pTemp);
	while(pPattern){m_vPatterns.push_back(pPattern);pPattern=strtok_r(NULL,";",&pTemp);}
	free(pPatternList);
	pPatternList=NULL;
	
	m_bOpenMode=true;
	if(Execute(piParent)!=DIALOG_OK){return false;}
	*psFile=m_sFile;
	return true;
}

bool CGameGUIFileDialog::SaveFile(IGameWindow *piParent,std::string sTitle,const char *psFilter,std::string *psFile,bool bOverWriteWarn)
{	
	m_sFile=*psFile;
	m_sTitle=sTitle;

	m_vPatterns.clear();
	char *pTemp=NULL;
	char *pPatternList=strdup(psFilter);
	char *pPattern=strtok_r(pPatternList,";",&pTemp);
	while(pPattern){m_vPatterns.push_back(pPattern);pPattern=strtok_r(NULL,";",&pTemp);}
	free(pPatternList);
	pPatternList=NULL;

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

void CGameGUIFileDialog::OnSelectionChanged(IGameGUIList *piControl,int nElement,std::string sElement)
{
  if(nElement!=-1 && m_piEDPath)
  {
    std::string sNormalized=NormalizePath(sElement);
	m_piEDPath->SetText(sNormalized);
	m_piEDPath->SetCursor(sNormalized.length());
  }
}

void CGameGUIFileDialog::OnSelectionDoubleCliked(IGameGUIList *piControl,int nElement,std::string sElement)
{
  if(m_piEDPath)
  {
    std::string sNormalized=NormalizePath(sElement);
	m_piEDPath->SetText(sNormalized);
	m_piEDPath->SetCursor(sNormalized.length());
  }
  ProcessSelect();
}