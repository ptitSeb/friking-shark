// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__428D9C9E_EBE0_4B74_B330_77774B0091AD__INCLUDED_)
#define AFX_STDAFX_H__428D9C9E_EBE0_4B74_B330_77774B0091AD__INCLUDED_

#define _WIN32_WINNT  0x0501
#define WINVER  0x0501

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <atlbase.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <string>
#include <crtdbg.h>

using namespace std;

bool operator<(const GUID &guid1,const GUID &guid2);

#include "LayoutLiblayoutconstraint.h"
#include "LayoutLiblayoutsegment.h"
#include "LayoutLiblayoutdimension.h"
#include "LayoutLiblayoutunit.h"
#include "LayoutLiblayout.h"
#include "LayoutLiblayoutwindow.h"
#include "LayoutLibsplitter.h"


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__428D9C9E_EBE0_4B74_B330_77774B0091AD__INCLUDED_)
