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


#pragma once

enum EStaticStructureState
{
	eStaticStructureState_Normal=ENTITY_STATE_BASE,
	eStaticStructureState_Destroyed
};

class CStaticStructureType: virtual public CEntityTypeBase, virtual public IStaticStructureTypeDesign
{
public:
	
	double m_dTimeFirstShotMin;
	double m_dTimeFirstShotMax;
	double m_dTimeBetweenShotsMin;
	double m_dTimeBetweenShotsMax;
	
	std::vector<SBBox> m_vVulnerableRegions;
	std::vector<SBBox> m_vProtectiveRegions;
	std::vector<SBBox> m_vProtectiveDestroyedRegions;
	
	
	IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);
	void InitializeEntity(CEntityBase *piEntity,unsigned int dwCurrentTime);

	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eStaticStructureState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()
	
	BEGIN_PROP_MAP(CStaticStructureType)
	PROP_CLASS_CHAIN(CEntityTypeBase)
		PROP_VALUE_FLAGS(m_dTimeFirstShotMin,"TimeFirstShotMin",500,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeFirstShotMax,"TimeFirstShotMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeBetweenShotsMin,"TimeBetweenShotsMin",2000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeBetweenShotsMax,"TimeBetweenShotsMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_vVulnerableRegions,"VulnerableRegions",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_vProtectiveRegions,"ProtectiveRegions",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_vProtectiveDestroyedRegions,"ProtectiveDestroyedRegions",MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();	
	
	
	// IStaticStructureTypeDesign
	
	void GetVulnerableRegions(std::vector<SBBox> *pvRegions);
	void SetVulnerableRegions(std::vector<SBBox> *pvRegions);
	void GetProtectiveRegions(std::vector<SBBox> *pvRegions);
	void SetProtectiveRegions(std::vector<SBBox> *pvRegions);
	void GetProtectiveDestroyedRegions(std::vector<SBBox> *pvRegions);
	void SetProtectiveDestroyedRegions(std::vector<SBBox> *pvRegions);
	
	CStaticStructureType();
	~CStaticStructureType();
};


class CStaticStructure: virtual public CEntityBase, virtual public IStaticStructure
{
	CStaticStructureType  *m_pType;
	unsigned int m_nConfiguredDamageType;
	unsigned int m_dwNextShotTime;
	bool 		 m_bFirstTimeVisible;
	double m_dRadius;
	
	IEntity *GetTarget();
protected:
	
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
	void OnAnimationEvent(string sEvent,string sParams);
	void OnDamage(double dDamage,IEntity *piAggresor);
public:

	const std::vector<SBBox> &GetVulnerableRegions();
	const std::vector<SBBox> &GetProtectiveRegions();
	
	void OnKilled();

	CStaticStructure(CStaticStructureType *pType,unsigned int dwCurrentTime);
};