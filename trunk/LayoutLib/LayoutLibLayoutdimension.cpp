#include "./stdafx.h"

CMRLayoutDimension::CMRLayoutDimension()
{
	//Put the default ones
	m_lcPrimary = NULL;
//	m_lcSecondary = NULL;
	m_lSecondarys.clear();

	m_iLastSecondary = -1;

	m_bResolved = false;
	m_pParentLayout = NULL;
	m_pParentUnit = NULL;

	m_idStep = GUID_NULL;

	m_bClip = false;

	m_bTrim = true;

	m_bHaveMinSize=false;
	m_bHaveMaxSize=false;


	SetPrimaryRestriction(new CMRLayoutPositionConstraint);
	m_bPrimarySet = false;
	
	m_iMinConstraint = SetSecondaryRestriction(new CMRLayoutMinSizeConstraint);
	m_iMaxConstraint = SetSecondaryRestriction(new CMRLayoutMaxSizeConstraint);
	m_iTrimConstraint = SetSecondaryRestriction(new CMRLayoutTrimSizeConstraint);

	m_lSecondarys[m_iMinConstraint]->m_bEnabled=false;
	m_lSecondarys[m_iMaxConstraint]->m_bEnabled=false;
	m_lSecondarys[m_iTrimConstraint]->m_bEnabled=true;

}

CMRLayoutDimension::~CMRLayoutDimension()
{
	if(m_lcPrimary){delete m_lcPrimary;m_lcPrimary=NULL;}

	map<int, CMRLayoutConstraint *>::iterator conIt;
	for(conIt = m_lSecondarys.begin();conIt != m_lSecondarys.end();conIt++)
	{
		if(conIt->second){delete conIt->second;}
	}
	m_lSecondarys.clear();
}

void CMRLayoutDimension::SetBounds(CMRLayoutSegment &bounds)
{
	m_lsBounds = bounds;
	m_lsOriginalBounds = bounds;
}

bool CMRLayoutDimension::Resolve(GUID step)
{
	if (m_bResolved) {return true;}

	_ASSERTE(step != m_idStep); //Circular dependency
	if (step == m_idStep)
	{
		return false;
	} 
	m_idStep = step;

	
//	if (m_lcSecondary == NULL)
	if (m_lSecondarys.size() ==0)
	{
		if(m_lcPrimary == NULL)
		{
			m_bResolved=true;
		}
		else
			{m_bResolved = m_lcPrimary->Resolve(step);}
	}
	else
	{
		BOOL resPrim = m_lcPrimary->Resolve(step); 
		BOOL resSec  = true;//m_lcSecondary->Resolve(step);
		map<int, CMRLayoutConstraint *>::iterator conIt;
		for(conIt = m_lSecondarys.begin();conIt != m_lSecondarys.end();conIt++)
		{
			if (!conIt->second->Resolve(step))
			{
				resSec=false;
				break;
			}
		}

		m_bResolved = (resPrim && resSec);
	}
/*	
	CMRLayoutSegment parentSegment(m_pParentLayout->m_rRelativeBounds, m_lsBounds.m_ldDirection);

//To be removed	
	bool changeSize=false;
	int newSize=0;

	if (m_bHaveMinSize && m_lcPrimary!=NULL)
	{
		if (m_lsBounds < m_iMinSize)
		{
			newSize = m_iMinSize;
			changeSize=true;
		}
	}
	if (m_bHaveMaxSize && m_lcPrimary!=NULL)
	{
		if (m_lsBounds > m_iMaxSize)
		{
			newSize = m_iMaxSize;
			changeSize=true;
		}
	}

	if (changeSize)
	{
		m_lsBounds.SizeFromFirst(newSize);
		m_lcPrimary->SetDirty();
		m_lcPrimary->Resolve(step);
	}

/*	if (m_bTrim)
	{
		m_lsBounds.TrimTo(parentSegment);
	}
*/
// Untill here
	return m_bResolved;
}

bool CMRLayoutDimension::IsResolved()
{
	return m_bResolved;
}

void CMRLayoutDimension::GetSegment(CMRLayoutSegment *bounds)
{
	*bounds = m_lsBounds;
}
void CMRLayoutDimension::SetDirty()
{
	if(m_lcPrimary!=NULL){m_lcPrimary->SetDirty();}
//	if(m_lcSecondary!=NULL){m_lcSecondary->SetDirty();}
	map<int, CMRLayoutConstraint *>::iterator conIt;
	for(conIt = m_lSecondarys.begin();conIt != m_lSecondarys.end();conIt++)
		{conIt->second->SetDirty();}

	m_lsBounds = m_lsOriginalBounds;
	m_bResolved = false;
}

void CMRLayoutDimension::SetPrimaryRestriction(CMRLayoutPositionConstraint *pPrimary)
{
	if(m_lcPrimary){delete m_lcPrimary;m_lcPrimary=NULL;}
	m_lcPrimary = pPrimary;
	m_lcPrimary->SetParentDimension(this, true);
	m_bPrimarySet = true;
}
int CMRLayoutDimension::SetSecondaryRestriction(CMRLayoutConstraint *pSecondary, int index)
{
	if (m_lcPrimary==NULL)
		{SetPrimaryRestriction(new CMRLayoutPositionConstraint);}

	int defaultIndex = pSecondary->GetPriority();
	map<int, CMRLayoutConstraint *>::iterator conIt;

	if (index > 0)  //Index manually specified -> look for the constraint, and if exists... overwrite it
	{
		conIt = m_lSecondarys.find(index);
		if (conIt != m_lSecondarys.end())
		{
			if(conIt->second){delete conIt->second;}
			m_lSecondarys.erase(conIt);
		}
	}
	else if (defaultIndex != -1) //Special priority -> put behind the last one in this priority
	{
		index = defaultIndex;
		while (1)
		{
			conIt = m_lSecondarys.find(index);
			if (conIt != m_lSecondarys.end())
				{index = conIt->first+1;}
			else
				{break;}
		}
	}
	else if (index <= -1 && defaultIndex == -1)
	{
		m_iLastSecondary++;	
		index = m_iLastSecondary;
	}


	pSecondary->SetParentDimension(this, false);
	m_lSecondarys[index] = pSecondary;

	return index;
}

void CMRLayoutDimension::SetSize(CMRLayoutSizeConstraint *pSize)
	{SetSecondaryRestriction(pSize);}


LayoutDirection CMRLayoutDimension::GetDirection()
{
	return m_lsBounds.m_ldDirection;
}

void CMRLayoutDimension::GetParentRelativeBound(CMRLayoutSegment &bound)
{
	bound.m_ldDirection = m_lsBounds.m_ldDirection;
	bound.FromRect(m_pParentLayout->m_rRelativeBounds);
};
