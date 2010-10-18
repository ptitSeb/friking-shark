#include "stdafx.h"

map<HWND, VaSplitterWindowData>	CMRSplitterManager::m_mWindows;

CMRSplitterManager::CMRSplitterManager()
{
	m_mWindows.clear();
}
CMRSplitterManager::~CMRSplitterManager()
{
	m_mWindows.clear();
}

bool CMRSplitterManager::RegisterSplitter(HWND hWnd, CMRSplitter *splitter)
{
	map<HWND, VaSplitterWindowData>::iterator wndIt;
	list<CMRSplitter *>::iterator splIt;
	
	wndIt = m_mWindows.find(hWnd);
	if(wndIt == m_mWindows.end()) //We don't have that window.
	{
		ULONG oldProc = GetWindowLong(hWnd, GWL_WNDPROC);
		m_mWindows[hWnd] = VaSplitterWindowData(oldProc);
		SetWindowLong(hWnd, GWL_WNDPROC, (ULONG)HostWindowProc);
		wndIt = m_mWindows.find(hWnd);
	}
	
	for(splIt = wndIt->second.m_mSplitters.begin(); splIt != wndIt->second.m_mSplitters.end(); splIt++)
	{
		if((*splIt) == splitter)
			{return TRUE;}
	}
		
	wndIt->second.m_mSplitters.push_back(splitter);
	return TRUE;
}
bool CMRSplitterManager::UnregisterSplitter(HWND hWnd, CMRSplitter *splitter)
{
	map<HWND, VaSplitterWindowData>::iterator wndIt;
	list<CMRSplitter *>::iterator splIt;

	wndIt = m_mWindows.find(hWnd);
	if(wndIt == m_mWindows.end()) //We don't have that window.
		{return FALSE;}

	for(splIt = wndIt->second.m_mSplitters.begin(); splIt != wndIt->second.m_mSplitters.end(); splIt++)
	{
		if((*splIt) == splitter)
			{break;}
	}
	if(splIt == wndIt->second.m_mSplitters.end()) //we do not have the splitter
		{return FALSE;}
		
	wndIt->second.m_mSplitters.erase(splIt);
	if(wndIt->second.m_mSplitters.size() == 0)
	{
		SetWindowLong(wndIt->first, GWL_WNDPROC, (ULONG)wndIt->second.m_oldWndProc);
		m_mWindows.erase(wndIt);
	}
	return TRUE;
}

LRESULT CALLBACK CMRSplitterManager::HostWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	map<HWND, VaSplitterWindowData>::iterator wndIt;
	list<CMRSplitter *>::iterator splIt;

	CMRSplitter *pSplitter = NULL;
	POINT cursorPoint;

	wndIt = m_mWindows.find(hWnd);
	if(wndIt == m_mWindows.end()) //We don't have that window.
		{return CallWindowProc(DefWindowProc, hWnd, uMsg, wParam, lParam);}

	if(uMsg == WM_ERASEBKGND || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE || uMsg == WM_PAINT || uMsg == WM_SETCURSOR) //Our Messages
	{
		//Splitters of that window
		bool bReleaseCapture = false;
		bool bMouseCaptured = false;

		bool bOverHorizontal = false;
		bool bOverVertical = false;
		bool bOverButton = false;
		for(splIt = wndIt->second.m_mSplitters.begin(); splIt != wndIt->second.m_mSplitters.end(); splIt++)
		{
			pSplitter = *splIt;
			cursorPoint.x = (short)LOWORD(lParam);
			cursorPoint.y = (short)HIWORD(lParam);

			if(uMsg == WM_LBUTTONDOWN)
			{
				pSplitter->OnLButtonDown(cursorPoint);
			}
			else if(uMsg == WM_LBUTTONUP)
			{
				pSplitter->OnLButtonUp(cursorPoint);
			}
			else if(uMsg == WM_MOUSEMOVE)
			{
				pSplitter->OnMouseMove(cursorPoint);
			}
			else if(uMsg == WM_PAINT)
			{
				pSplitter->OnPaint();
			}
			else if(uMsg == WM_ERASEBKGND)
			{
				pSplitter->OnEraseBackGround();
			}
			else if(uMsg == WM_SETCURSOR)
			{
				::GetCursorPos(&cursorPoint);
				::ScreenToClient(hWnd, &cursorPoint);
				if(pSplitter->CursorOverSplitter(cursorPoint) && !pSplitter->GetCheck())
				{
					if(pSplitter->m_bVertical){bOverVertical = true;}
					else{bOverHorizontal = true;}
				}
				else if(pSplitter->CursorOverButton(cursorPoint))
				{
					bOverButton = true;
				}

			}
		}

		if(bReleaseCapture){::ReleaseCapture();}
		if(uMsg == WM_SETCURSOR && (bOverVertical || bOverHorizontal || bOverButton))
		{
			HCURSOR hCur = NULL;
			if(bOverButton){hCur = ::LoadCursor(NULL, IDC_HAND);}
			else if(bOverVertical && bOverHorizontal){hCur = ::LoadCursor(NULL, IDC_SIZEALL);}
			else if(bOverVertical){hCur = ::LoadCursor(NULL, IDC_SIZEWE);}
			else if(bOverHorizontal){hCur = ::LoadCursor(NULL, IDC_SIZENS);}
			::SetCursor(hCur);
			return TRUE;
		}

	}
	return CallWindowProc((WNDPROC)(wndIt->second.m_oldWndProc), hWnd, uMsg, wParam, lParam);
}




//////////////////////////////////////
// CMRSplitter
//

CMRSplitter::CMRSplitter()
{
	m_hWnd = NULL;
	m_bEnabled = true;

	m_bPaint = false;
	m_bHaveButton = false;

	m_bShadowDrawn = false;
	m_bChecked = false;

	m_dwSplitterWidth = 4;
	m_dwButtonHeight = 40;

	m_dwStyle = 0;

	m_fSplitter = 50.0f;
	m_fCheckedSplitter = 100.0f;
	m_fUncheckedSplitter = m_fSplitter;


	m_bEnableColorKey = true;
	m_bAutoColorKey = true;
	m_colorKey = 0;

	m_hCheckedBitmapHandle = NULL;
	m_hCheckedBitmapDC = CreateCompatibleDC(NULL);
	memset(&m_checkedBitmapInfo, 0, sizeof(m_checkedBitmapInfo));
	
	m_hUncheckedBitmapHandle = NULL;
	m_hUncheckedBitmapDC = CreateCompatibleDC(NULL);
	memset(&m_uncheckedBitmapInfo, 0, sizeof(m_uncheckedBitmapInfo));

}

CMRSplitter::~CMRSplitter()
{
	Destroy();
}


void CMRSplitter::Init(HWND hWnd, bool vertical, float initialValue, float minSplitter, float maxSplitter, DWORD splitterWidth)
{
	m_hWnd = hWnd;

	m_fSplitter = initialValue;
	m_fMinSplitter = (DWORD)minSplitter;
	m_fMaxSplitter = (DWORD)maxSplitter;
	m_bVertical = vertical;

//	m_dwSplitterWidth = splitterWidth;
	SetSplitterWidth(splitterWidth);
	m_dwSplitterWidth /= 2;
	m_dwSplitterWidth *= 2;
	if(m_dwSplitterWidth <= 0){m_dwSplitterWidth = 2;}

	m_fCheckedSplitter = 100.0f;
	m_fUncheckedSplitter = m_fSplitter;

	m_bPaint = false;
	m_bHaveButton = false;
	m_dwButtonHeight = 40;

	m_dwStyle = 0;

	m_bLButtonDown = false;
	m_bShadowDrawn = false;
	m_bChecked = false;

	memset(&m_rLastInvertedRect,0,sizeof(m_rLastInvertedRect));
	CMRSplitterManager::RegisterSplitter(m_hWnd, this);
}

void CMRSplitter::Destroy()
{
	CMRSplitterManager::UnregisterSplitter(m_hWnd, this);
	if(m_hCheckedBitmapHandle != NULL){DeleteObject(m_hCheckedBitmapHandle);}
	if(m_hUncheckedBitmapHandle != NULL){DeleteObject(m_hUncheckedBitmapHandle);}
	DeleteDC(m_hCheckedBitmapDC);
	DeleteDC(m_hUncheckedBitmapDC);
}

bool CMRSplitter::GetCheck()
{
	return m_bChecked;
}
void CMRSplitter::SetCheck(bool bChecked)
{
	if(m_bChecked != bChecked)
	{
		m_bChecked = bChecked;
		if(m_bChecked)
		{
			m_fUncheckedSplitter = m_fSplitter;
			m_fSplitter = m_fCheckedSplitter;
		}
		else
		{
			m_fSplitter = m_fUncheckedSplitter;
		}
	}
	LPARAM lParam = m_bChecked?MRMF_CHECK:MRMF_UNCHECK | MRMF_POSITION;
	PostMessage(m_hWnd,MRM_SPLITTERCHANGE, (WPARAM)this, lParam);
}

void CMRSplitter::OnLButtonDown(POINT cursorPoint) 
{
	if(m_hWnd == NULL || !m_bEnabled){return;}	
	if(CursorOverSplitter(cursorPoint) &&  !m_bChecked)
	{
		DrawSplitterShadow(true, &cursorPoint);
		m_bLButtonDown = true;
		if(GetCapture() != m_hWnd){::SetCapture(m_hWnd);}
	}
	else if(CursorOverButton(cursorPoint))
	{
		SetCheck(!m_bChecked);
	}
	return;
}
bool CMRSplitter::OnLButtonUp(POINT cursorPoint) 
{
	if(m_hWnd == NULL || !m_bEnabled){return FALSE;}	

	if (m_bLButtonDown)
	{
		m_bLButtonDown=false;	
		::ReleaseCapture();
		
		RECT r;
		::GetClientRect(m_hWnd, &r);
		DrawSplitterShadow(false);

		float temp = 0;
		if(m_bVertical)
		{
			temp = (float)(100.0 * (float)cursorPoint.x / (float)(r.right-r.left));
		}
		else
		{
			temp = (float)(100.0 * (float)cursorPoint.y / (float)(r.bottom-r.top));
		}

		if(temp > (float)m_fMaxSplitter){temp = (float)m_fMaxSplitter;}
		else if(temp < (float)m_fMinSplitter){temp = (float)m_fMinSplitter;}

		m_fSplitter = temp;


//		::InvalidateRect(m_hWnd, NULL, TRUE);
		LPARAM lParam = MRMF_POSITION;
		PostMessage(m_hWnd,MRM_SPLITTERCHANGE, (WPARAM)this, lParam);
		return TRUE;
	}
	
	return FALSE;
}
void CMRSplitter::OnMouseMove(POINT cursorPoint)
{
	if(m_hWnd == NULL || !m_bEnabled){return;}	

	if(m_bLButtonDown)
	{
		DrawSplitterShadow(true, &cursorPoint);
	}
	return;
}

void CMRSplitter::OnPaint()
{
	if(m_hWnd == NULL || !m_bEnabled || m_bLButtonDown){return;}	
	HDC hdc = ::GetDC(m_hWnd);
	RECT r = {0};
	GetSplitterRect(&r);

	if(m_bPaint)
	{
		if(m_dwStyle & VSS_SplitterBorder)
			{::DrawEdge(hdc, &r, EDGE_ETCHED, BF_RECT | BF_MIDDLE);}
		else
			{::DrawEdge(hdc, &r, 0, BF_MIDDLE);}
		
		if(m_dwStyle & (VSS_SplitterStripes | VSS_SplitterSparseStripes | VSS_SplitterSingleStripe))
		{
			int maxStripeCount = m_dwSplitterWidth / 4;
			int stripeCount = maxStripeCount;
			float stride = 4.0;
			float width = ((stripeCount - 1) * stride) + 3; 
			float offset = ((float)m_dwSplitterWidth - width) / 2;

			if(m_dwStyle & VSS_SplitterSparseStripes)
			{
				if(stripeCount > 3){stripeCount = maxStripeCount / 2;}
				else if(stripeCount == 3){stripeCount = 2;}

				if(stripeCount == 1){offset = (float)((m_dwSplitterWidth - 3)/ 2);}
				else if(stripeCount == 2)
				{
					stride = (float)(((float)m_dwSplitterWidth) / 2.0);
					offset = (float)((stride / 2.0) - 1.0);
					width = ((stripeCount - 1) * stride) + 4;
				}
				else
				{
					stride = 8.0;
					width = ((stripeCount - 1) * stride) + 4;
					offset = ((float)m_dwSplitterWidth - width) / 2;
				}
			}
			else if(m_dwStyle & VSS_SplitterSingleStripe)
			{
				stripeCount = 1;
				offset = (float)((m_dwSplitterWidth - 3)/ 2);
			}

			if(m_bVertical)
			{
				RECT tempRect = r;
				tempRect.left += (LONG)offset;
				tempRect.right = tempRect.left + 3;
				for(int x = 0; x < stripeCount; x++)
				{
					::DrawEdge(hdc, &tempRect, BDR_RAISEDOUTER, BF_RECT | BF_MIDDLE | BF_SOFT);
					tempRect.left += (LONG)stride;
					tempRect.right = tempRect.left + 3;
				}
			}
			else
			{
				RECT tempRect = r;
				tempRect.top += (LONG)offset;
				tempRect.bottom = tempRect.top + 3;
				for(int x = 0; x < stripeCount; x++)
				{
					::DrawEdge(hdc, &tempRect, BDR_RAISEDOUTER, BF_RECT | BF_MIDDLE | BF_SOFT);
					tempRect.top += (LONG)stride;
					tempRect.bottom = tempRect.top + 3;
				}
			}
		}
	}
	else
	{
		::DrawEdge(hdc, &r, 0, BF_MIDDLE);
	}
	if(m_bHaveButton)
	{
		RECT buttonRect = {0};
		GetButtonRect(&buttonRect);

		if(m_hUncheckedBitmapHandle)
		{
			if(m_bChecked)
				{DrawButton(hdc, &buttonRect, m_hCheckedBitmapDC, m_checkedBitmapInfo);}
			else
				{DrawButton(hdc, &buttonRect, m_hUncheckedBitmapDC, m_uncheckedBitmapInfo);}
		}
		else
		{::DrawEdge(hdc, &buttonRect, EDGE_RAISED, BF_RECT | BF_MIDDLE);}
	}

	::ReleaseDC(m_hWnd, hdc);

	::ValidateRect(m_hWnd, &r);
	return;
}

void CMRSplitter::OnEraseBackGround()
{
	RECT r = {0};
	GetSplitterRect(&r);
	::InvalidateRect(m_hWnd, &r, FALSE);
	return;
}

void CMRSplitter::SetStyle(DWORD dwStyle)
{
	m_dwStyle = dwStyle;

	m_bPaint = HAVE_TO_PAINT_SPLITER(m_dwStyle)?true:false;
	m_bHaveButton = (m_dwStyle & VSS_Button)?true:false;
}
DWORD CMRSplitter::GetStyle()
{
	return m_dwStyle;
}
void CMRSplitter::Enable(bool bEnable)
{
	m_bEnabled = bEnable;
	PostMessage(m_hWnd,MRM_SPLITTERCHANGE, (WPARAM)this, 0);
}
bool CMRSplitter::IsEnabled()
{
	return m_bEnabled;
}

void CMRSplitter::SetCheckedSplitterPos(float fSplitterPos)
{
	m_fCheckedSplitter = fSplitterPos;
}
void CMRSplitter::SetSplitterWidth(DWORD splitterWidth)
{
	m_dwSplitterWidth = splitterWidth;
	if(m_dwSplitterWidth < 4){m_dwSplitterWidth = 4;}
}
void CMRSplitter::SetSplitterButtonHeight(DWORD buttonHeight)
{
	m_dwButtonHeight = buttonHeight;
	if(m_dwButtonHeight < 40){m_dwButtonHeight = 40;}
}

bool CMRSplitter::CursorOverSplitter(POINT cursorPoint)
{
	if(m_hWnd == NULL || !m_bEnabled){return false;}
	if(!CursorOverButton(cursorPoint))
	{
		RECT r = {0};
		GetSplitterRect(&r);
		return PtInRect(&r, cursorPoint)?true:false;
	}
	return false;
}

bool CMRSplitter::CursorOverButton(POINT cursorPoint)
{
	if(m_hWnd == NULL || !m_bEnabled || !m_bHaveButton){return false;}
	RECT r = {0};
	GetButtonRect(&r);
	r.left--;
	r.right++;
	return PtInRect(&r, cursorPoint)?true:false;
}

void CMRSplitter::GetSplitterRect(RECT *pRect)
{
	if(m_hWnd == NULL){return;}
	::GetClientRect(m_hWnd, pRect);

	float splitterPos = 0.0;
	if(m_bVertical)
	{
		splitterPos = ((float)(pRect->right-pRect->left) * m_fSplitter / (float)100.0);
		if(m_bChecked && m_fSplitter == 100.0f)
		{
			pRect->left		= (LONG)(splitterPos - m_dwSplitterWidth);
			pRect->right	= (LONG)splitterPos;
		}
		else if(m_bChecked && m_fSplitter == 0.0f)
		{
			pRect->left		= (LONG)splitterPos;
			pRect->right	= (LONG)(splitterPos + m_dwSplitterWidth);
		}
		else
		{
			pRect->left		= (LONG)(splitterPos - ((float)m_dwSplitterWidth / 2.0));
			pRect->right	= (LONG)(splitterPos + ((float)m_dwSplitterWidth / 2.0));
		}
	}
	else
	{
		splitterPos = ((pRect->bottom-pRect->top) * m_fSplitter / 100);
		if(m_bChecked && m_fSplitter == 100.0f)
		{
			pRect->top		= (LONG)(splitterPos - m_dwSplitterWidth);
			pRect->bottom	= (LONG)(splitterPos);
		}
		else if(m_bChecked && m_fSplitter == 0.0f)
		{
			pRect->top		= (LONG)(splitterPos);
			pRect->bottom	= (LONG)(splitterPos + m_dwSplitterWidth);
		}
		else
		{
			pRect->top		= (LONG)(splitterPos - (((float)m_dwSplitterWidth / 2.0) - 1.0));
			pRect->bottom	= (LONG)(splitterPos + (((float)m_dwSplitterWidth / 2.0) - 1.0));
		}
	}
}

void CMRSplitter::GetButtonRect(RECT *pRect)
{
	GetSplitterRect(pRect);
	if(m_bVertical)
	{
		int buttonCenter = (pRect->bottom + pRect->top) / 2;
		pRect->top = buttonCenter - (m_dwButtonHeight / 2);
		pRect->bottom = buttonCenter + (m_dwButtonHeight / 2);
	}
	else
	{
		int buttonCenter = (pRect->right + pRect->left) / 2;
		pRect->left = buttonCenter - (m_dwButtonHeight / 2);
		pRect->right = buttonCenter + (m_dwButtonHeight / 2);
	}

}


void CMRSplitter::DrawSplitterShadow(bool bDraw, POINT *pCursorPoint)
{
	RECT r;
	::GetClientRect(m_hWnd, &r);
	HDC hdc = ::GetDC(m_hWnd);

	if(!bDraw && m_bShadowDrawn)
	{
		::InvertRect(hdc, &m_rLastInvertedRect);
		m_bShadowDrawn = false;
		memset(&m_rLastInvertedRect, 0, sizeof(m_rLastInvertedRect));
		return;
	}

	if(bDraw)
	{
		float temp = 0;
		float splitterPos = 0;
		if(m_bVertical)
		{
			if(pCursorPoint)
			{
				temp = (float)(100.0 * ((float)pCursorPoint->x) / (float)(r.right-r.left));
				if(temp > m_fMaxSplitter){temp = (float)m_fMaxSplitter;}
				else if(temp < m_fMinSplitter){temp = (float)m_fMinSplitter;}
				splitterPos = ((r.right-r.left) * temp / 100);
			}
			else
				{splitterPos = ((r.right-r.left) * m_fSplitter / 100);}
			r.left = (LONG)(splitterPos - (m_dwSplitterWidth / 2));
			r.right= (LONG)(splitterPos + (m_dwSplitterWidth / 2));
		}
		else
		{
			if(pCursorPoint)
			{
				temp = (float)(100.0 * ((float)pCursorPoint->y) / (float)(r.bottom-r.top));
				if(temp > m_fMaxSplitter){temp = (float)m_fMaxSplitter;}
				else if(temp < m_fMinSplitter){temp = (float)m_fMinSplitter;}
				splitterPos = ((r.bottom-r.top) * temp / 100);
			}
			else
				{splitterPos = ((r.bottom-r.top) * m_fSplitter / 100);}
			r.top   = (LONG)(splitterPos - (m_dwSplitterWidth / 2));
			r.bottom= (LONG)(splitterPos + (m_dwSplitterWidth / 2));
		}

		if(memcmp(&m_rLastInvertedRect, &r, sizeof(r)) != 0)
		{
			::InvertRect(hdc, &m_rLastInvertedRect);
			m_rLastInvertedRect = r;
			::InvertRect(hdc, &m_rLastInvertedRect);
			::ReleaseDC(m_hWnd, hdc);
			m_bShadowDrawn = true;
		}
	}
}


//Button Bitmaps Stuff
void CMRSplitter::SetCheckedBitmap(HBITMAP bitmap)
{
	if(m_hCheckedBitmapHandle != NULL){DeleteObject(m_hCheckedBitmapHandle);m_hCheckedBitmapHandle=NULL;}
	m_hCheckedBitmapHandle = (HBITMAP)CopyImage(bitmap, IMAGE_BITMAP, 0, 0, DUPLICATE_SAME_ACCESS);
	if(m_hCheckedBitmapHandle == NULL)
	{
		::SelectObject(m_hCheckedBitmapDC, bitmap);
		::GetObject(bitmap, sizeof(m_checkedBitmapInfo), &m_checkedBitmapInfo);
	}
	else
	{
		::SelectObject(m_hCheckedBitmapDC, m_hCheckedBitmapHandle);
		::GetObject(m_hCheckedBitmapHandle, sizeof(m_checkedBitmapInfo), &m_checkedBitmapInfo);
		if(m_bAutoColorKey){m_colorKey = GetPixel(m_hCheckedBitmapDC, 0, 0);}
	}
}

void CMRSplitter::SetUncheckedBitmap(HBITMAP bitmap)
{
	if(m_hUncheckedBitmapHandle != NULL){DeleteObject(m_hUncheckedBitmapHandle);m_hUncheckedBitmapHandle=NULL;}
	m_hUncheckedBitmapHandle = (HBITMAP)CopyImage(bitmap, IMAGE_BITMAP, 0, 0, DUPLICATE_SAME_ACCESS);
	if(m_hUncheckedBitmapHandle == NULL)
	{
		::SelectObject(m_hUncheckedBitmapDC, bitmap);
		::GetObject(bitmap, sizeof(m_uncheckedBitmapInfo), &m_uncheckedBitmapInfo);
	}
	else
	{
		::SelectObject(m_hUncheckedBitmapDC, m_hUncheckedBitmapHandle);
		::GetObject(m_hUncheckedBitmapHandle, sizeof(m_uncheckedBitmapInfo), &m_uncheckedBitmapInfo);
		if(m_bAutoColorKey){m_colorKey = GetPixel(m_hUncheckedBitmapDC, 0, 0);}
	}
}

void CMRSplitter::SetBitmaps(HBITMAP hBitmap, bool flipSides, bool adjustToBitmapSize)
{
}

void CMRSplitter::SetBitmaps(string fileName, bool flipSides, bool adjustToBitmapSize)
{
}

void CMRSplitter::SetBitmaps(HINSTANCE hInst, UINT resId, bool flipSides, bool adjustToBitmapSize)
{
}

void CMRSplitter::AdjustToBitmapSize()
{
	if(m_hUncheckedBitmapHandle)
	{
		if(m_bVertical)
		{
			SetSplitterWidth(m_uncheckedBitmapInfo.bmWidth + 2);
			m_dwButtonHeight = m_uncheckedBitmapInfo.bmHeight;
		}
		else
		{
			SetSplitterWidth(m_uncheckedBitmapInfo.bmHeight + 2);
			m_dwButtonHeight = m_uncheckedBitmapInfo.bmWidth;
		}
	}
}

void CMRSplitter::DrawButton(HDC hdc, RECT *pRect, HDC bitmap, BITMAP bitmapInfo)
{
	int hdcW = pRect->right - pRect->left;
	int hdcH = pRect->bottom - pRect->top;

	if(m_bEnableColorKey)
	{
		HDC tempDC = ::CreateCompatibleDC(hdc);
		HBITMAP tempBMP = ::CreateCompatibleBitmap(hdc, hdcW, hdcH);
		::TransparentBlt(hdc, pRect->left, pRect->top, hdcW, hdcH, bitmap,0,0,bitmapInfo.bmWidth,bitmapInfo.bmHeight,m_colorKey);

		::DeleteDC(tempDC);			
		::DeleteObject(tempBMP);
	}
	else
		{::StretchBlt(hdc, pRect->left, pRect->top, hdcW, hdcH, bitmap,0,0,bitmapInfo.bmWidth,bitmapInfo.bmHeight,SRCCOPY);}

}
