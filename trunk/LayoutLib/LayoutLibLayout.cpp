#include "stdafx.h"

template <typename T>
void CopyDeque(deque<T> *pDest,deque<T> *pSource)
{
	pDest->clear();
	for(DWORD x=0;x<pSource->size();x++){pDest->push_back((*pSource)[x]);}
}

CMRLayout::CMRLayout(char *alias)
{
	CoCreateGuid(&m_idLayout);
	sprintf(m_alias, alias);
}

CMRLayout::~CMRLayout()
{
	ClearLayout();
}

void CMRLayout::AddUnit(CMRLayoutUnit *pUnit)
{
	if (pUnit == NULL){return;}
	map<GUID, CMRLayoutUnit *>::iterator unitIt;
	unitIt = m_mUnits.find(pUnit->m_idUnit);
	if(unitIt != m_mUnits.end())
		{return;}

	m_mUnits[*pUnit] = pUnit;
	m_dUnits.push_back(pUnit);
	pUnit->SetParentLayout(this);

	SetDirty();
};

void CMRLayout::RemoveUnit(GUID unitId)
{
	map<GUID, CMRLayoutUnit *>::iterator unitIt;
	unitIt = m_mUnits.find(unitId);
	if(unitIt != m_mUnits.end())
		{m_mUnits.erase(unitId);}

	CMRLayoutUnit *pTempUnit;
	deque<CMRLayoutUnit *> dTempUnits;
	for(DWORD x = 0; x < m_dUnits.size(); x++)
	{
		pTempUnit = m_dUnits[x];
		if((*pTempUnit) != unitId)
			{dTempUnits.push_back(pTempUnit);}
	}
	CopyDeque(&m_dUnits, &dTempUnits);
};


void CMRLayout::ClearLayout()
{
	CMRLayoutUnit *pLayoutUnit = NULL;
	map<GUID, CMRLayoutUnit *>::iterator unitIt;
	for(unitIt=m_mUnits.begin();unitIt!=m_mUnits.end();unitIt++)
	{
		pLayoutUnit = unitIt->second;
		if(pLayoutUnit){pLayoutUnit->RestoreOriginalBounds();}
		delete unitIt->second;
	}
	m_mUnits.clear();
	m_dUnits.clear();
};

void CMRLayout::SetDirty()
{
	for(DWORD x = 0; x < m_dUnits.size(); x++)
	{
		m_dUnits[x]->SetDirty();
	}
}

bool CMRLayout::Resolve(RECT bounds, GUID step)
{
	GUID newStep = step;
	if (newStep == GUID_NULL)
		{CoCreateGuid(&newStep);}


	SetDirty();

	m_rBounds = bounds;
	m_rRelativeBounds.left = 0;
	m_rRelativeBounds.top = 0;
	m_rRelativeBounds.right = bounds.right - bounds.left;
	m_rRelativeBounds.bottom = bounds.bottom - bounds.top;

	for(DWORD x = 0; x < m_dUnits.size(); x++)
	{
		if(!m_dUnits[x]->Resolve(newStep)){return false;}
	}

	return true;
};

void CMRLayout::GetRects(list<CMRLayoutUnitBounds> &rects, bool recursive)
{
	list<CMRLayoutUnitBounds> tempList;

	for(DWORD x = 0; x < m_dUnits.size(); x++)
	{
		if(recursive)
			{m_dUnits[x]->GetBoundsRectList(tempList);}
		else
		{
			tempList.push_back(m_dUnits[x]->GetUnitBounds());
		}

	}

	MakeAbsolute(tempList);
	rects.splice(rects.end(),tempList);

	//Clip(rects);
};

void CMRLayout::GetRelativeRects(list<CMRLayoutUnitBounds> &rects, bool recursive)
{
	for(DWORD x = 0; x < m_dUnits.size(); x++)
	{
		if(recursive)
			{m_dUnits[x]->GetBoundsRectList(rects);}
		else
			{rects.push_back(m_dUnits[x]->GetUnitBounds());}
	}
};

void CMRLayout::MakeAbsolute(list<CMRLayoutUnitBounds> &rects)
{
	list<CMRLayoutUnitBounds>::iterator rectIt;

	POINT desp = {0};
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

void CMRLayout::Clip(list<CMRLayoutUnitBounds> &rects)
{
	list<CMRLayoutUnitBounds>::iterator boundsIt;
	list<CMRLayoutUnitBounds>::iterator internalIt;

	HRGN	hRegion = 0;

	for (boundsIt = rects.begin(); boundsIt != rects.end(); boundsIt++)
	{
		
		hRegion = CreateRectRgnIndirect(&(boundsIt->m_rBounds));
		_ASSERTE (hRegion != 0);
	
		boundsIt++;
		for (internalIt = boundsIt; internalIt != rects.end(); internalIt++)
		{
			if(RectInRegion(hRegion, &(internalIt->m_rBounds))) //Collision
			{
				ClipRects((boundsIt->m_rBounds), (internalIt->m_rBounds));
			}
		}
		DeleteObject(hRegion);
		boundsIt--;
	}
}


void CMRLayout::ClipRects(RECT &r1, RECT &r2)
{
	CMRLayoutSegment h1,h2,v1,v2;

	CMRLayoutSegment::SegmentsFromRect(h1, v1, r1);
	CMRLayoutSegment::SegmentsFromRect(h2, v2, r2);

	CMRLayoutSegment::Intersect (h1, h2);
	CMRLayoutSegment::Intersect (v1, v2);
	
	CMRLayoutSegment::RectFromSegments(r1, h1, v1);
	CMRLayoutSegment::RectFromSegments(r2, h2, v2);
}

bool operator<(const GUID &guid1,const GUID &guid2) 
{
	return memcmp(&guid1,&guid2,sizeof(guid1))<0;
}