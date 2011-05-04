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

class CParticleWhirlpoolModifierType:public CParticleEmitterModifierType
{
public:

	CVector m_vRotationAxis;
	double m_dStartRadius;
	double m_dEndRadius;
	double m_dStartAngle;
	unsigned int m_nStartTime;
	unsigned int m_nEndTime;
	unsigned int m_nLoops;
	
    IParticleEmitterModifier *CreateInstance(unsigned int dwCurrentTime);

	// Propiedades Persistentes
	
	BEGIN_PROP_MAP(CParticleWhirlpoolModifierType)
		PROP_CLASS_CHAIN(CParticleEmitterModifierType)
		PROP_VALUE_FLAGS(m_dStartRadius,"StartRadius",5,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dEndRadius,"EndRadius",100,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nStartTime,"StartTime",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nEndTime,"EndTime",5000,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_dStartAngle,"StartAngle",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nLoops,"Loops",4,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_vRotationAxis,"RotationAxis",AxisPosY,MRPF_NORMAL|MRPF_OPTIONAL)
		END_PROP_MAP()

    CParticleWhirlpoolModifierType();
    ~CParticleWhirlpoolModifierType();
};

class CParticleWhirlpoolModifier:public IParticleEmitterModifier
{
	unsigned int m_nCreationTime;
	CParticleWhirlpoolModifierType *m_pType;
	map<IParticleEmitter *,unsigned int> m_mEmitters;
	
public:

	void AddEmitter(IParticleEmitter *pEmitter);
	
	void ProcessEmitter(IParticleEmitter *piEmitter,IParticleSystem *piSystem,unsigned int dwCurrentTime,double dInterval);

	CParticleWhirlpoolModifier(CParticleWhirlpoolModifierType *pType,unsigned long dwCurrentTime);
};
