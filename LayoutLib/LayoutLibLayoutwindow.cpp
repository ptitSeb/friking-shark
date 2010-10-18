
#include "stdafx.h"


////////////////////////////
/// CMRChildLayoutWindow
CMRChildLayoutWindow::CMRChildLayoutWindow(char *alias) : CMRLayoutUnitWindow(NULL, alias)
{
	SetChildLayout(new CMRLayoutWindow);
}
CMRChildLayoutWindow::~CMRChildLayoutWindow()
{
}
void CMRChildLayoutWindow::AddUnit(CMRLayoutUnit *pUnit)
{
	if(m_vlChildLayout){m_vlChildLayout->AddUnit(pUnit);}
}
void CMRChildLayoutWindow::RemoveUnit(GUID unitId)
{
	if(m_vlChildLayout){m_vlChildLayout->RemoveUnit(unitId);}
}
void CMRChildLayoutWindow::ClearLayout()
{
	if(m_vlChildLayout){m_vlChildLayout->ClearLayout();}
}


////////////////////////////
/// CMRLayoutUnitWindow
CMRLayoutUnitWindow::CMRLayoutUnitWindow(HWND window, char *alias) : CMRLayoutUnit(alias)
{
	m_hWindow = window;

	RECT bounds = {0};
	if(m_hWindow){::GetWindowRect(m_hWindow, &bounds);}
	SetBoundsRect(bounds);
}


CMRLayoutUnitBoundsWindow CMRLayoutUnitWindow::GetUnitBounds()
{
	CMRLayoutUnitBoundsWindow temp(CMRLayoutUnit::GetUnitBounds(), m_hWindow);
	return temp;
}

RECT CMRLayoutUnitWindow::GetBoundsRectList(list<CMRLayoutUnitBoundsWindow> &rects)
{
	RECT r = GetBoundsRect();
	if (!(m_bHorizontalResolved && m_bVerticalResolved && m_bChildResolved)){return r;}

	if (m_vlChildLayout!=NULL)
		{((CMRLayoutWindow *)m_vlChildLayout)->GetRects(rects, true);}
	
	CMRLayoutUnitBoundsWindow temp(m_idUnit, r, m_hWindow);
	rects.push_back(temp);

	return r;
}

void CMRLayoutUnitWindow::RestoreOriginalBounds()
{
	CMRLayoutUnit::RestoreOriginalBounds();
	if(m_hWindow)
	{
		RECT r = GetOriginalBoundsRect();
		::SetWindowPos(m_hWindow, NULL, 0, 0, r.right - r.left, r.bottom - r.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOSENDCHANGING | SWP_NOACTIVATE);
	}
}


//High level shortcuts
void CMRLayoutUnitWindow::Abs(GUID property, int value)
{
	if(property == ceLeft || property == ceRight || property == ceTop || property == ceBottom) //Position
	{
		if(property == ceLeft || property == ceRight) //Horizontal
		{
			if(!m_ldHorizontal.m_bPrimarySet)
				{m_ldHorizontal.SetPrimaryRestriction(new CMRLayoutAbsolutePositionConstraint(property==ceLeft?ceFirstComponent:ceSecondComponent, value));}
			else
				{m_ldHorizontal.SetSecondaryRestriction(new CMRLayoutAbsolutePositionConstraint(property==ceLeft?ceFirstComponent:ceSecondComponent, value));}
		}
		else //Vertical
		{
			if(!m_ldVertical.m_bPrimarySet)
				{m_ldVertical.SetPrimaryRestriction(new CMRLayoutAbsolutePositionConstraint(property==ceTop?ceFirstComponent:ceSecondComponent, value));}
			else
				{m_ldVertical.SetSecondaryRestriction(new CMRLayoutAbsolutePositionConstraint(property==ceTop?ceFirstComponent:ceSecondComponent, value));}
		}
	}
	else if(property == csWidth || property == csHeight) //size
	{
		if(property == csWidth) //Horizontal
		{
			m_ldHorizontal.SetSecondaryRestriction(new CMRLayoutAbsoluteSizeConstraint(value));
		}
		else //Vertical
		{
			m_ldVertical.SetSecondaryRestriction(new CMRLayoutAbsoluteSizeConstraint(value));
		}
	}

	else //-> wrong parameters
	{
		_ASSERTE(false);
	}
}

/*static const GUID crLeftOf = crBefore;
static const GUID crRightOf = crAfter;
static const GUID crAbove = crBefore;
static const GUID crBelow = crAfter;
static const GUID crMiddleOf = { 0xf4945515, 0x8b98, 0x48fd, { 0x84, 0x8d, 0x9c, 0x9e, 0xc4, 0x7, 0x80, 0x89 } };*/


void CMRLayoutUnitWindow::Rel(GUID property, GUID relation, GUID relativeUnit, GUID otherProperty, int value, int value2)
{
	if(property == ceLeft || property == ceRight || property == ceTop || property == ceBottom) //Position
	{
		if(relation != crBefore &&  relation != crAfter && relation != crMiddleOf && relation != crSameAs && relation != crPercent)
		{
			_ASSERTE(!(relation != crBefore &&  relation != crAfter && relation != crMiddleOf && relation != crSameAs && relation != crPercent));
			return;
		}

		if(property == ceLeft || property == ceRight) //Horizontal
		{
			if(!m_ldHorizontal.m_bPrimarySet)
			{
				m_ldHorizontal.SetPrimaryRestriction(
							   new CMRLayoutRelativePositionConstraint(
									property==ceLeft?ceFirstComponent:ceSecondComponent, relation, relativeUnit, 
									otherProperty==ceLeft?ceFirstComponent:ceSecondComponent, value, value2));
			}
			else
			{
				m_ldHorizontal.SetSecondaryRestriction(
							   new CMRLayoutRelativePositionConstraint(
									property==ceLeft?ceFirstComponent:ceSecondComponent, relation, relativeUnit, 
									otherProperty==ceLeft?ceFirstComponent:ceSecondComponent, value, value2));
			}
		}
		else //Vertical
		{
			if(!m_ldVertical.m_bPrimarySet)
			{
				m_ldVertical.SetPrimaryRestriction(
							   new CMRLayoutRelativePositionConstraint(
									property==ceTop?ceFirstComponent:ceSecondComponent, relation, relativeUnit, 
									otherProperty==ceTop?ceFirstComponent:ceSecondComponent, value, value2));
			}
			else
			{
				m_ldVertical.SetSecondaryRestriction(
							   new CMRLayoutRelativePositionConstraint(
									property==ceTop?ceFirstComponent:ceSecondComponent, relation, relativeUnit, 
									otherProperty==ceTop?ceFirstComponent:ceSecondComponent, value, value2));
			}
		}
	}
	else if(property == csWidth || property == csHeight) //size
	{
		if(relation != crSameAs &&  relation != crPercent)
		{
			_ASSERTE(!(relation != crSameAs &&  relation != crPercent));
			return;
		}

		if(property == csWidth) //Horizontal
		{
			m_ldHorizontal.SetSecondaryRestriction(new CMRLayoutRelativeSizeConstraint(relation, relativeUnit, otherProperty, value));
		}
		else //Vertical
		{
			m_ldVertical.SetSecondaryRestriction(new CMRLayoutRelativeSizeConstraint(relation, relativeUnit, otherProperty, value));
		}
	}

	else //-> wrong parameters
	{
		_ASSERTE(false);
	}
}

void CMRLayoutUnitWindow::Rel(GUID property, GUID relation, CMRLayoutUnit *pRelativeUnit, GUID otherProperty, int value, int value2)
{
	if(pRelativeUnit){Rel(property, relation, *pRelativeUnit, otherProperty, value, value2);}
}


void CMRLayoutUnitWindow::Splitter(GUID property, float *splitterValue, int offset)
{
	if(property == ceLeft || property == ceRight || property == ceTop || property == ceBottom) //Position
	{
		if(property == ceLeft || property == ceRight) //Horizontal
		{
			if(!m_ldHorizontal.m_bPrimarySet)
			{
				m_ldHorizontal.SetPrimaryRestriction(new CMRLayoutSplitterPositionConstraint(property, splitterValue, offset));
			}
			else
			{
				m_ldHorizontal.SetSecondaryRestriction(new CMRLayoutSplitterPositionConstraint(property, splitterValue, offset));
			}
		}
		else //Vertical
		{
			if(!m_ldVertical.m_bPrimarySet)
			{
				m_ldVertical.SetPrimaryRestriction(new CMRLayoutSplitterPositionConstraint(property, splitterValue, offset));
			}
			else
			{
				m_ldVertical.SetSecondaryRestriction(new CMRLayoutSplitterPositionConstraint(property, splitterValue, offset));
			}
		}
	}
	else //-> wrong parameters
	{
		_ASSERTE(false);
	}
}
void CMRLayoutUnitWindow::Splitter(GUID property, CMRSplitter &splitter, int offset)
{
	if(property == ceLeft || property == ceRight || property == ceTop || property == ceBottom) //Position
	{
		if(property == ceLeft || property == ceRight) //Horizontal
		{
			if(!m_ldHorizontal.m_bPrimarySet)
			{
				m_ldHorizontal.SetPrimaryRestriction(new CMRLayoutSplitterPositionConstraint(property, &splitter, offset));
			}
			else
			{
				m_ldHorizontal.SetSecondaryRestriction(new CMRLayoutSplitterPositionConstraint(property, &splitter, offset));
			}
		}
		else //Vertical
		{
			if(!m_ldVertical.m_bPrimarySet)
			{
				m_ldVertical.SetPrimaryRestriction(new CMRLayoutSplitterPositionConstraint(property, &splitter, offset));
			}
			else
			{
				m_ldVertical.SetSecondaryRestriction(new CMRLayoutSplitterPositionConstraint(property, &splitter, offset));
			}
		}
	}
	else //-> wrong parameters
	{
		_ASSERTE(false);
	}
}

void CMRLayoutUnitWindow::LeftOf(GUID relativeUnit, int value)
{
	Rel(ceRight, crLeftOf, relativeUnit, ceLeft, value);
}
void CMRLayoutUnitWindow::LeftOf(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){LeftOf(*pRelativeUnit, value);}
}

void CMRLayoutUnitWindow::InLeftOf(GUID relativeUnit, int value)
{
	Rel(ceLeft, crRightOf, relativeUnit, ceLeft, value);
}
void CMRLayoutUnitWindow::InLeftOf(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){InLeftOf(*pRelativeUnit, value);}
}

void CMRLayoutUnitWindow::RightOf(GUID relativeUnit, int value)
{
	Rel(ceLeft, crRightOf, relativeUnit, ceRight, value);
}
void CMRLayoutUnitWindow::RightOf(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){RightOf(*pRelativeUnit, value);}
}

void CMRLayoutUnitWindow::InRightOf(GUID relativeUnit, int value)
{
	Rel(ceRight, crLeftOf, relativeUnit, ceRight, value);
}
void CMRLayoutUnitWindow::InRightOf(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){InRightOf(*pRelativeUnit, value);}
}


void CMRLayoutUnitWindow::Above(GUID relativeUnit, int value)
{
	Rel(ceBottom, crAbove, relativeUnit, ceTop, value);
}
void CMRLayoutUnitWindow::Above(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){Above(*pRelativeUnit, value);}
}

void CMRLayoutUnitWindow::InTopOf(GUID relativeUnit, int value)
{
	Rel(ceTop, crBelow, relativeUnit, ceTop, value);
}
void CMRLayoutUnitWindow::InTopOf(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){InTopOf(*pRelativeUnit, value);}
}

void CMRLayoutUnitWindow::Below(GUID relativeUnit, int value)
{
	Rel(ceTop, crBelow, relativeUnit, ceBottom, value);
}
void CMRLayoutUnitWindow::Below(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){Below(*pRelativeUnit, value);}
}

void CMRLayoutUnitWindow::InBottomOf(GUID relativeUnit, int value)
{
	Rel(ceBottom, crAbove, relativeUnit, ceBottom, value);
}
void CMRLayoutUnitWindow::InBottomOf(CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){InBottomOf(*pRelativeUnit, value);}
}

void CMRLayoutUnitWindow::SameAs(GUID property, GUID relativeUnit, int value)
{
	Rel(property, crSameAs, relativeUnit, property, value);
}
void CMRLayoutUnitWindow::SameAs(GUID property, CMRLayoutUnit *pRelativeUnit, int value)
{
	if(pRelativeUnit){SameAs(property, *pRelativeUnit, value);}
}


//////////////////////////////
///// CMRLayoutWindow
CMRLayoutWindow::CMRLayoutWindow(char *alias) : CMRLayout(alias)
{

};

CMRLayoutWindow::CMRLayoutWindow(HWND window, char *alias) : CMRLayout(alias)
{
	m_hWindow = window;
	ClearLayout();
}

CMRLayoutWindow::~CMRLayoutWindow()
{
	ClearLayout();
}


void CMRLayoutWindow::Layout(int _cx,int _cy)
{
	RECT r = {0};
	if(m_hWindow){::GetClientRect(m_hWindow, &r);}

	bool res = Resolve(r);

	_ASSERT(res);

	
	list<CMRLayoutUnitBoundsWindow> bounds;
	list<CMRLayoutUnitBoundsWindow>::iterator boundsIt;

	GetRects(bounds, true);
	HDWP defer = BeginDeferWindowPos(bounds.size());
	int cx=0, cy=0;
	for (boundsIt = bounds.begin(); boundsIt != bounds.end(); boundsIt++)
	{
		cx = boundsIt->m_rBounds.right - boundsIt->m_rBounds.left;
		cy = boundsIt->m_rBounds.bottom - boundsIt->m_rBounds.top;
		if(boundsIt->m_hWindow){DeferWindowPos(defer, boundsIt->m_hWindow, 0,boundsIt->m_rBounds.left, boundsIt->m_rBounds.top, cx, cy, 0);}
	}
	EndDeferWindowPos(defer);
	if(m_hWindow){::InvalidateRect(m_hWindow, NULL, TRUE);}
}

void CMRLayoutWindow::GetRects(list<CMRLayoutUnitBoundsWindow> &rects, bool recursive)
{
	map<GUID, CMRLayoutUnit *>::iterator unitIt;
	list<CMRLayoutUnitBoundsWindow> tempList;

	tempList.clear();

	for(unitIt=m_mUnits.begin();unitIt!=m_mUnits.end();unitIt++)
	{
		if(recursive)
			{((CMRLayoutUnitWindow *)(unitIt->second))->GetBoundsRectList(tempList);}
		else
		{
			tempList.push_back(((CMRLayoutUnitWindow *)(unitIt->second))->GetUnitBounds());
		}
	}
	MakeAbsolute(tempList);

//	Clip(tempList);
	
	rects.splice(rects.end(),tempList);


}

void CMRLayoutWindow::MakeAbsolute(list<CMRLayoutUnitBoundsWindow> &rects)
{
	list<CMRLayoutUnitBoundsWindow>::iterator rectIt;

	POINT desp;
	desp.x=m_rBounds.left;
	desp.y=m_rBounds.top;

	for(rectIt=rects.begin();rectIt!=rects.end();rectIt++)
	{
		rectIt->m_rBounds.left+=desp.x;
		rectIt->m_rBounds.right+=desp.x;
		rectIt->m_rBounds.top+=desp.y;
		rectIt->m_rBounds.bottom+=desp.y;
	}

}

void CMRLayoutWindow::Clip(list<CMRLayoutUnitBoundsWindow> &rects)
{
	list<CMRLayoutUnitBoundsWindow>::iterator boundsIt;
	list<CMRLayoutUnitBoundsWindow>::iterator internalIt;

	HRGN	hRegion = 0;
	RECT *r1, *r2;

	for (boundsIt = rects.begin(); boundsIt != rects.end(); boundsIt++)
	{
		r1 = &(boundsIt->m_rBounds);
		hRegion = CreateRectRgnIndirect(r1);
		_ASSERTE (hRegion != 0);
	
		boundsIt++;
		for (internalIt = boundsIt; internalIt != rects.end(); internalIt++)
		{
			r2 = &(internalIt->m_rBounds);
			if(RectInRegion(hRegion, r2)) //Collision
			{
				ClipRects(*r1, *r2);
			}
		}
		DeleteObject(hRegion);
		boundsIt--;
	}
}








/////////////////////////


bool CMRLayoutTextSizeConstraintWindow::Resolve(GUID step)
{
	int textSize = 0, textLen = 0;
	HWND hWindow = 0;
	char temp[512];
	bool ret=false;


	hWindow = ((CMRLayoutUnitWindow *)(m_ldParentDimension->m_pParentUnit))->m_hWindow;
	textLen = ::GetWindowText(hWindow, temp, 512);


	if (textLen != 0)
	{
		HDC	dc = 0;
		RECT r;
		HFONT font = 0;

		::GetClientRect(hWindow, &r);
		dc = ::GetDC(hWindow);

		font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		::SelectObject(dc, font);
		::GetClientRect(hWindow, &r);
		::DrawText(dc, temp, textLen, &r, DT_CALCRECT | DT_SINGLELINE);
		::ReleaseDC(hWindow, dc);

		textSize = ((int)CMRLayoutSegment(r,m_ldParentDimension->GetDirection()) + 2);
	}
	else
	{
		textSize = 2;
	}
	

	m_fValue +=textSize;
	ret = CMRLayoutAbsoluteSizeConstraint::Resolve(step);
	m_fValue -= textSize;	

	return ret;
}


bool CMRLayoutSplitterPositionConstraint::Resolve(GUID step)
{
	bool ret = false;
	float offset = m_fValue;
	float tempValue = 0;
	int splitterWidth = 0;

	if(m_pSplitterValue != NULL)
	{
		tempValue = *m_pSplitterValue;
	}
	else if(m_pSplitter != NULL)
	{
		tempValue = m_pSplitter->m_fSplitter;
		if(!m_pSplitter->IsEnabled()){splitterWidth = 0;}
		else if(m_pSplitter->GetCheck() && (m_pSplitter->m_fSplitter == 100.0f || m_pSplitter->m_fSplitter == 0.0f)){splitterWidth = (m_pSplitter->m_dwSplitterWidth);}
		else{splitterWidth = (m_pSplitter->m_dwSplitterWidth) / 2;}
	}


	m_fValue = tempValue;
	ret = CMRLayoutRelativePositionConstraint::Resolve(step);

	//We apply the offset (in pixels)
	if(m_bPrimary)  //Moving all the object to match the constraint
	{
		m_ldParentDimension->m_lsBounds += (int)offset;
		if(m_ceMyEdge == ceFirstComponent)
		{
			m_ldParentDimension->m_lsBounds.m_iFirst += splitterWidth;
		}
		else if(m_ceMyEdge == ceSecondComponent)
		{
			m_ldParentDimension->m_lsBounds.m_iSecond -= splitterWidth;
		}
	}
	else //Secondary -> sizing (we leave alone the other component)
	{
		if(m_ceMyEdge == ceFirstComponent)
		{
			m_ldParentDimension->m_lsBounds.m_iFirst += (int)offset;
			m_ldParentDimension->m_lsBounds.m_iFirst += splitterWidth;
		}
		else if(m_ceMyEdge == ceSecondComponent)
		{
			m_ldParentDimension->m_lsBounds.m_iSecond += (int)offset;
			m_ldParentDimension->m_lsBounds.m_iSecond -= splitterWidth;
		}
		else{ret = false;}
	}

	m_fValue = offset;
	return ret;
}
