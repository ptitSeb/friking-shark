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

DECLARE_CUSTOM_WRAPPER1(CParticleSystemTypeWrapper,IParticleSystemType,m_piParticleSystemType)

class CParticleSystemAnimationObjectType: public CAnimationObjectTypeBase, 
public virtual IParticleSystemAnimationObjectTypeDesign,
public virtual IAnimationObjectTypePositionDesign
{
public:
	
	CParticleSystemTypeWrapper  m_ParticleSystemType;
	CVector              		m_vPosition;
	unsigned int                m_nStartTime;
	unsigned int                m_nEndTime;
	bool                		m_bTrackEntity;
	
	IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);
	std::string 	  GetAnimationObjectDescription();
	
	BEGIN_PROP_MAP(CParticleSystemAnimationObjectType)
	PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
	PROP(m_ParticleSystemType,"ParticleSystem")
	PROP_FLAGS(m_vPosition,"Position",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_bTrackEntity,"TrackEntity",false,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nEndTime,"EndTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
	
	
	// IParticleSystemAnimationObjectTypeDesignn
	
	void GetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig);
	void SetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig);
	void SetParticleSystemType(IParticleSystemType *piParticleSystemType);
	void GetParticleSystemType(IParticleSystemType **ppiParticleSystemType);
	
	// IAnimationObjectTypePositionDesign
	
	CVector GetPosition();
	void	SetPosition(CVector vPosition);
	
	CParticleSystemAnimationObjectType();
	~CParticleSystemAnimationObjectType();
};


class CParticleSystemAnimationObject: public CAnimationObjectBase
{
	CParticleSystemAnimationObjectType  *m_pType;
	IParticleSystem                     *m_piParticleSystem;
	bool                                 m_bPositionAndAnglesInitialized;
	
	void UpdatePositionAndAngles();
	void CheckActivation(unsigned int dwCurrentTime);
	
public:
	
	void Activate(unsigned int dwCurrentTime);
	void Deactivate();
	
	bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
	void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);
	
	CParticleSystemAnimationObject(CParticleSystemAnimationObjectType *pType,IAnimation *piAnimation);
	~CParticleSystemAnimationObject();
};
