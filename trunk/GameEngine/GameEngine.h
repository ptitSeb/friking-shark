// GameEngine.h : main header file for the GAMEENGINE application
//

#if !defined(AFX_GAMEENGINE_H__5B824DCE_E2DA_4162_A337_4638C145BBED__INCLUDED_)
#define AFX_GAMEENGINE_H__5B824DCE_E2DA_4162_A337_4638C145BBED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "resource.h"		// main symbols
#include <string>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CGameEngineApp:
// See GameEngine.cpp for the implementation of this class
//

class CGameEngineApp
{
public:
	CGameEngineApp();

	void Run();
	void InterpretCommandLine(std::string sExecutableFolder,std::vector<std::string> &vParams);
};

extern CGameEngineApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEENGINE_H__5B824DCE_E2DA_4162_A337_4638C145BBED__INCLUDED_)

