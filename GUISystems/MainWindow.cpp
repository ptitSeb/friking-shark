#include "StdAfx.h"
#include "resource.h"
#include "GameRuntimeLib.h"
#include "GameGUILib.h"
#include "GUISystems.h"
#include ".\mainwindow.h"

DECLARE_CUSTOM_WRAPPER1(CGameGUIManagerWrapper,IGameGUIManager,m_piInterface);

extern CSystemModuleHelper *g_pSystemModuleHelper;

CMainWindow::CMainWindow(void)
{
	m_eStage=eInterfaceStage_MainMenu;
	m_dBackgroundAlpha=0;
	m_bVisible=true;
}

CMainWindow::~CMainWindow(void)
{
}
bool CMainWindow::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	if(bOk)
	{
		CGameGUIManagerWrapper guiManager;
		guiManager.Attach("GameGUI","GUIManager");
		IGameWindow *piMainWindow=guiManager.m_piInterface->GetMainWindow();
		InitWindow(piMainWindow,false);
		Show(true);
		REL(piMainWindow);
	}
	return bOk;
}
	
bool CMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);
	if(bResult)
	{
		HICON hIcon=LoadIcon(g_pSystemModuleHelper->GetInstance(),MAKEINTRESOURCE(IDR_MAINFRAME));
		m_Viewport.Attach("GameGUI","Viewport");
		m_Viewport.m_piViewport->SetCaption("Friking Shark");
		m_Viewport.m_piViewport->SetIcon(hIcon);

		SGameRect sRect;
		sRect.x=0;
		sRect.y=0;
		sRect.w=1;
		sRect.h=1;
		m_eReferenceSystem=eGameGUIReferenceSystem_Relative;
		SetRect(&sRect);

		bResult=m_GUIConfigFile.Open("Resources\\Scripts\\GameGUI.cfg");

		if(bResult)
		{
			m_BackgroundWindow.Create("GameGUI","CBackgroundWindow","Background");
			m_MainMenuDialog.Create("GameGUI","CMainMenu","MainMenu");
			m_GameMenuDialog.Create("GameGUI","CGameMenu","GameMenu");
			m_ConfirmationDialog.Create("GameGUI","CConfirmationDialog","ConfirmationDialog");
			m_GameInterfaceWindow.Create("GameGUI","CGameInterface","GameInterface");
			m_GameOverDialog.Create("GameGUI","CGameSimpleDialog","GameOverDialog");
			m_CongratulationsDialog.Create("GameGUI","CGameSimpleDialog","CongratulationsDialog");

			m_BackgroundWindow.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\BackgroundWindow"));
			m_MainMenuDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\MainMenu"));
			m_GameMenuDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\GameMenu"));
			m_GameInterfaceWindow.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\GameInterface"));
			m_GameOverDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\GameOverDialog"));
			m_CongratulationsDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\CongratulationsDialog"));

			SUBSCRIBE_TO_CAST(m_GameInterfaceWindow.m_piObject,IGameInterfaceWindowEvents);
		}

		if(bResult)
		{
			m_BackgroundWindow.m_piWindow->InitWindow(this,false);
			m_BackgroundWindow.m_piWindow->Show(true);
			m_BackgroundWindow.m_piWindow->Activate(false);
		}
		m_piGUIManager->SetFocus(this);
	}
	return bResult;
}

void CMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==VK_ESCAPE)
	{
		*pbProcessed=true;

		if(m_eStage==eInterfaceStage_MainMenu)
		{
			int result=m_MainMenuDialog.m_piDialog->Execute(this);
			if(result==eMainMenuAction_Exit)
			{
				m_piGUIManager->ExitGUILoop();
			}
			else if(result==eMainMenuAction_NewGame)
			{
				m_eStage=eInterfaceStage_Playing;

				m_piGUIManager->ShowMouseCursor(false);
				m_BackgroundWindow.m_piWindow->Show(false);
				m_GameInterfaceWindow.m_piInterfaceWindow->InitWindow(this,false);
				m_GameInterfaceWindow.m_piInterfaceWindow->Show(true);
				m_GameInterfaceWindow.m_piInterfaceWindow->Activate(false);
			}
			m_piGUIManager->SetFocus(this);
		}
		else if(m_eStage==eInterfaceStage_Playing)
		{
			m_piGUIManager->ShowMouseCursor(true);
			m_GameInterfaceWindow.m_piInterfaceWindow->Freeze(true);
			int result=m_GameMenuDialog.m_piDialog->Execute(this);
			m_GameInterfaceWindow.m_piInterfaceWindow->Freeze(false);
			if(result==eGameMenuAction_EndGame)
			{
				m_GameInterfaceWindow.m_piInterfaceWindow->DestroyWindow();
				m_BackgroundWindow.m_piWindow->Show(true);
				m_eStage=eInterfaceStage_MainMenu;
			}
			else
			{
				m_piGUIManager->ShowMouseCursor(false);
			}
			m_piGUIManager->SetFocus(this);
		}
	}
}

void CMainWindow::OnScenarioFinished(eScenarioFinishedReason eReason)
{
	m_piGUIManager->ShowMouseCursor(true);
	m_GameInterfaceWindow.m_piInterfaceWindow->Freeze(true);
	if(eReason==eScenarioFinishedReason_Completed)
	{
		m_CongratulationsDialog.m_piDialog->Execute(this);
	}
	else
	{
		m_GameOverDialog.m_piDialog->Execute(this);
	}
	m_GameInterfaceWindow.m_piInterfaceWindow->Freeze(false);
	m_GameInterfaceWindow.m_piInterfaceWindow->DestroyWindow();
	m_BackgroundWindow.m_piWindow->Show(true);
	m_eStage=eInterfaceStage_MainMenu;
}
