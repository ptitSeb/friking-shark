#include "./stdafx.h"

///////////////////////////
////  CMRLayoutConstraint
CMRLayoutConstraint::CMRLayoutConstraint()
{
	m_ldParentDimension = NULL;
	m_bResolved = false;
	m_bPrimary = true;
	m_bEnabled = true;
}

CMRLayoutConstraint::~CMRLayoutConstraint(){}

RECT CMRLayoutConstraint::GetParentLayoutBounds()
{
	return m_ldParentDimension->m_pParentLayout->m_rRelativeBounds;
}

CMRLayoutConstraint *CMRLayoutConstraint::GetParentPrimaryRestriction()
{
	return m_ldParentDimension->m_lcPrimary;
}

void CMRLayoutConstraint::SetDirty()
{
	m_bResolved = false;
}




////////////////////////////////////////
//// CMRLayoutPositionConstraint
CMRLayoutPositionConstraint::CMRLayoutPositionConstraint(): CMRLayoutConstraint()
{};

bool CMRLayoutPositionConstraint::Resolve(GUID step)
	{return true;}


//////////////////////////////////////////
//// CMRLayoutAbsolutePositionConstraint
bool CMRLayoutAbsolutePositionConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	
	bool ret = true;

//	RECT parentBounds = GetParentLayoutBounds();
//	CMRLayoutSegment parentSegment(parentBounds, m_ldParentDimension->m_lsBounds.m_ldDirection);
	
	if(m_bPrimary)  //Moving all the object to match the constraint
	{
		if	(m_ceMyEdge == ceFirstComponent)
			{m_ldParentDimension->m_lsBounds.MoveFirstTo((int)m_fValue);}
		else if(m_ceMyEdge == ceSecondComponent)
			{m_ldParentDimension->m_lsBounds.MoveSecondTo((int)m_fValue);}
		else {ret = false;}
	}
	else //Secondary -> sizing (we leave alone the other component)
	{
		if	(m_ceMyEdge == ceFirstComponent)
			{m_ldParentDimension->m_lsBounds.m_iFirst = (int)m_fValue;}
		else if(m_ceMyEdge == ceSecondComponent)
			{m_ldParentDimension->m_lsBounds.m_iSecond = (int)m_fValue;}
		else {ret = false;}
	}

	m_bResolved=ret;
	return ret;
}


//////////////////////////////////////
//// CMRLayoutRelativePositionConstraint
bool CMRLayoutRelativePositionConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	
	if(m_ceMyEdge != ceFirstComponent && m_ceMyEdge != ceSecondComponent && m_ceItsEdge != ceFirstComponent && m_ceItsEdge != ceSecondComponent)
	{
		_ASSERTE(!(m_ceMyEdge != ceFirstComponent && m_ceMyEdge != ceSecondComponent && m_ceItsEdge != ceFirstComponent && m_ceItsEdge != ceSecondComponent));
		return false;
	}
	
	bool ret = true;
	CMRLayoutSegment relSegment;
	float value = m_fValue;

	LayoutDirection direction = m_ldParentDimension->GetDirection();

	//GetRelativeUnit (or parent)
	if(m_idRelativeUnit == vlParent)
	{
		RECT parentBounds = GetParentLayoutBounds();
		relSegment = CMRLayoutSegment(parentBounds, direction);
	}
	else
	{
		map<GUID, CMRLayoutUnit *>::iterator unitIt;
		unitIt = m_ldParentDimension->m_pParentLayout->m_mUnits.find(m_idRelativeUnit);
		if(unitIt == m_ldParentDimension->m_pParentLayout->m_mUnits.end()){return false;}

		if(!(unitIt->second->GetDimensionBounds(relSegment, direction, step))){return false;}
	}


	if(m_crRelation == crAfter || m_crRelation == crBefore || m_crRelation == crSameAs)
	{
		if(m_crRelation == crBefore) {value = -m_fValue;};

		if(m_ceItsEdge == ceFirstComponent)
			{value = relSegment.m_iFirst + value;}
		else if(m_ceItsEdge == ceSecondComponent)
			{value = relSegment.m_iSecond + value;}
	}
	else if(m_crRelation == crMiddleOf)
	{
		value += (relSegment.m_iFirst + relSegment.m_iSecond)/2;
	}
	else if(m_crRelation == crPercent)
	{
		value = (float)(((float)relSegment * (float)m_fValue / 100.0) + (float)m_fValue2);
	}
	else
	{
		return false;
	}

	if(m_bPrimary)  //Moving all the object to match the constraint
	{
		if	(m_ceMyEdge == ceFirstComponent)
			{m_ldParentDimension->m_lsBounds.MoveFirstTo((int)value);}
		else if(m_ceMyEdge == ceSecondComponent)
			{m_ldParentDimension->m_lsBounds.MoveSecondTo((int)value);}
		else {ret = false;}
	}
	else //Secondary -> sizing (we leave alone the other component)
	{
		if	(m_ceMyEdge == ceFirstComponent)
			{m_ldParentDimension->m_lsBounds.m_iFirst = (int)value;}
		else if(m_ceMyEdge == ceSecondComponent)
			{m_ldParentDimension->m_lsBounds.m_iSecond = (int)value;}
		else {ret = false;}
	}

	m_bResolved=ret;
	return ret;
}



bool CMRLayoutBetweenPositionConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	
	bool ret = true;
	CMRLayoutSegment temp;
	CMRLayoutSegment newBounds;

	LayoutDirection direction = m_ldParentDimension->GetDirection();
	
	//GetRelativeUnit (or parent)
	if(m_idRelativeUnit1 == vlParent || m_idRelativeUnit2 == vlParent)
		{return false;}
	else
	{
		map<GUID, CMRLayoutUnit *>::iterator unitIt;
		unitIt = m_ldParentDimension->m_pParentLayout->m_mUnits.find(m_idRelativeUnit1);
		if(unitIt == m_ldParentDimension->m_pParentLayout->m_mUnits.end())
			{return false;}
		if(!(unitIt->second->GetDimensionBounds(temp, direction, step)))
			{return false;}
		
		newBounds.m_iFirst = temp.m_iSecond + m_iValue1;

		unitIt = m_ldParentDimension->m_pParentLayout->m_mUnits.find(m_idRelativeUnit2);
		if(unitIt == m_ldParentDimension->m_pParentLayout->m_mUnits.end())
			{return false;}
		if(!(unitIt->second->GetDimensionBounds(temp, direction, step)))
			{return false;}

		newBounds.m_iSecond = temp.m_iFirst - m_iValue2;
	}
	
	m_ldParentDimension->m_lsBounds = newBounds;

	m_bResolved=ret;
	return ret;
}


//////////////////////////////////////
//// CMRLayoutAbsoluteSizeConstraint
bool CMRLayoutAbsoluteSizeConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	bool ret = true;
	CMRLayoutConstraint *primary=NULL;

	m_ldParentDimension->m_lsBounds.SizeFromFirst((int)m_fValue);

	primary = GetParentPrimaryRestriction();
	if(primary==NULL){return ret;}
	primary->SetDirty();
	primary->Resolve(step);

	m_bResolved=ret;
	return ret;
}


//////////////////////////////////////
//// CMRLayoutRelativeSizeConstraint
bool CMRLayoutRelativeSizeConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	bool ret = true;
	float value = m_fValue;
	CMRLayoutSegment relSegment;

	LayoutDirection direction;

	if(m_csItsSize == csWidth)
		{direction = LD_Horizontal;}
	else if(m_csItsSize == csHeight)
		{direction = LD_Vertical;}
	else
		{return false;}
	
	//GetRelativeUnit (or parent)
	if(m_idRelativeUnit == vlParent)
	{
		RECT parentBounds = GetParentLayoutBounds();
		relSegment = CMRLayoutSegment(parentBounds, direction);
	}
	else
	{
		map<GUID, CMRLayoutUnit *>::iterator unitIt;
		unitIt = m_ldParentDimension->m_pParentLayout->m_mUnits.find(m_idRelativeUnit);
		if(unitIt == m_ldParentDimension->m_pParentLayout->m_mUnits.end()){return false;}

		if(!(unitIt->second->GetDimensionBounds(relSegment, direction, step))){return false;}
	}


	if(m_crRelation == crSameAs)
	{
		value += relSegment;
	}
	else if(m_crRelation == crPercent)
	{
		value = (float)((float)relSegment * (float)m_fValue / 100.0);
	}
	else
	{
		return false;
	}

	
	m_ldParentDimension->m_lsBounds.SizeFromFirst((int)value);
	CMRLayoutConstraint *primary=NULL;
	primary = GetParentPrimaryRestriction();
	if(primary==NULL){return ret;}
	primary->SetDirty();
	primary->Resolve(step);

	m_bResolved=ret;
	return ret;
}

bool CMRLayoutTrimSizeConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	
	CMRLayoutSegment parentBound;
	m_ldParentDimension->GetParentRelativeBound(parentBound);
//	CMRLayoutSegment parentSegment(parentBounds, m_ldParentDimension->m_lsBounds.m_ldDirection);
	m_ldParentDimension->m_lsBounds.TrimTo(parentBound);

	return true;
};

bool CMRLayoutMinSizeConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	
	if(m_ldParentDimension->m_lsBounds >= m_fValue)
	{
		m_bResolved = true;
		return true;
	}

	return CMRLayoutAbsoluteSizeConstraint::Resolve(step);
};

bool CMRLayoutMaxSizeConstraint::Resolve(GUID step)
{
	if(!m_bEnabled){return true;}
	if(m_bResolved){return true;}
	
	if(m_ldParentDimension->m_lsBounds <= m_fValue)
	{
		m_bResolved = true;
		return true;
	}

	return CMRLayoutAbsoluteSizeConstraint::Resolve(step);
};
