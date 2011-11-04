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

