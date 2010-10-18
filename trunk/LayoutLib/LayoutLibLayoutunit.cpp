
#include "stdafx.h"

CMRLayoutUnit::CMRLayoutUnit(char *alias)
{
	CoCreateGuid(&m_idUnit);
	m_vlParentLayout = NULL;
	m_vlChildLayout = NULL;

	m_bHorizontalResolved = false;
	m_bVerticalResolved = false;
	m_bChildResolved = true;

	m_ldHorizontal.m_pParentUnit = this;
	m_ldVertical.m_pParentUnit = this;
	
	sprintf(m_alias, alias);
}

CMRLayoutUnit::CMRLayoutUnit(RECT bounds, char *alias)
{
	CoCreateGuid(&m_idUnit);
	m_vlParentLayout = NULL;
	m_vlChildLayout = NULL;

	m_bHorizontalResolved = false;
	m_bVerticalResolved = false;
	m_bChildResolved = true;

	sprintf(m_alias, alias);

	SetBoundsRect(bounds);
}

CMRLayoutUnit::~CMRLayoutUnit()
{	
	if(m_vlChildLayout){delete m_vlChildLayout;m_vlChildLayout=NULL;}
}

bool CMRLayoutUnit::Resolve(GUID step)
{
	if (m_bHorizontalResolved && m_bVerticalResolved) {return true;}

	m_bHorizontalResolved = m_ldHorizontal.Resolve(step);
	m_bVerticalResolved = m_ldVertical.Resolve(step);

	if (m_vlChildLayout!=NULL && !m_bChildResolved)
	{
		RECT r = GetBoundsRect();
		m_bChildResolved = m_vlChildLayout->Resolve(r);
	}
	else
		{m_bChildResolved=true;}

	return m_bHorizontalResolved && m_bVerticalResolved && m_bChildResolved;
}

bool CMRLayoutUnit::GetDimensionBounds(CMRLayoutSegment &bounds, LayoutDirection direction, GUID step)
{
	bool res = false;
	if (direction == LD_Horizontal)
	{
		if (!(res = m_ldHorizontal.IsResolved()))
		{
			res = m_ldHorizontal.Resolve(step);
		}
		m_ldHorizontal.GetSegment(&bounds);
	}
	else
	{
		if (!(res = m_ldVertical.IsResolved()))
		{
			res = m_ldVertical.Resolve(step);
		}
		m_ldVertical.GetSegment(&bounds);
	}

	return res;
}

bool CMRLayoutUnit::IsResolved()
{
	return (m_ldHorizontal.IsResolved() && m_ldVertical.IsResolved() && m_bChildResolved);
}
void CMRLayoutUnit::SetDirty()
{
	m_bHorizontalResolved = false;
	m_bVerticalResolved = false;

	m_ldHorizontal.SetDirty();
	m_ldVertical.SetDirty();

	m_bChildResolved = false;
}


void CMRLayoutUnit::SetBoundsRect(RECT bounds)
{
	m_ldHorizontal.SetBounds(CMRLayoutSegment(bounds, LD_Horizontal));
	m_ldVertical.SetBounds(CMRLayoutSegment(bounds, LD_Vertical));
}

RECT CMRLayoutUnit::GetBoundsRect()
{
	RECT r = {0};
	CMRLayoutSegment horizSegment, vertSegment;

	if (!(m_bHorizontalResolved && m_bVerticalResolved)){memset(&r, 0, sizeof(RECT)); return r;}

	m_ldHorizontal.GetSegment(&horizSegment);
	m_ldVertical.GetSegment(&vertSegment);

	r.left = horizSegment.m_iFirst;r.right = horizSegment.m_iSecond;
	r.top = vertSegment.m_iFirst;r.bottom = vertSegment.m_iSecond;
	return r;
}

RECT CMRLayoutUnit::GetOriginalBoundsRect()
{
	RECT r = {0};
	CMRLayoutSegment horizSegment, vertSegment;

	horizSegment = m_ldHorizontal.m_lsOriginalBounds;
	vertSegment = m_ldVertical.m_lsOriginalBounds;

	r.left = horizSegment.m_iFirst;r.right = horizSegment.m_iSecond;
	r.top = vertSegment.m_iFirst;r.bottom = vertSegment.m_iSecond;
	return r;
}


CMRLayoutUnitBounds CMRLayoutUnit::GetUnitBounds()
{
	CMRLayoutUnitBounds temp;

	temp.m_rOriginalBounds = GetOriginalBoundsRect();

	if (!(m_bHorizontalResolved && m_bVerticalResolved)){return temp;}

	temp.m_rBounds = GetBoundsRect();
	temp.m_idUnit = m_idUnit;
	temp.m_bHorizontalClip = m_ldHorizontal.GetClip();
	temp.m_bVerticalClip = m_ldVertical.GetClip();

	return temp;
}


RECT CMRLayoutUnit::GetBoundsRectList(list<CMRLayoutUnitBounds> &rects)
{
	RECT r = GetBoundsRect();
	if (!(m_bHorizontalResolved && m_bVerticalResolved && m_bChildResolved)){return r;}

	if (m_vlChildLayout!=NULL)
		{m_vlChildLayout->GetRects(rects, true);}
	
	CMRLayoutUnitBounds temp(m_idUnit, r);
	rects.push_back(temp);

	return r;
}

void CMRLayoutUnit::SetParentLayout(CMRLayout *parentLayout)
{
	m_vlParentLayout = parentLayout;
	m_ldHorizontal.m_pParentLayout=parentLayout;
	m_ldVertical.m_pParentLayout=parentLayout;
}

void CMRLayoutUnit::SetChildLayout(CMRLayout *childLayout)
{
	if(m_vlChildLayout){delete m_vlChildLayout;m_vlChildLayout=NULL;}
	m_vlChildLayout = childLayout;
}

void CMRLayoutUnit::RestoreOriginalBounds()
{

}

CMRLayoutUnit::operator GUID()
{
	return m_idUnit;
}
