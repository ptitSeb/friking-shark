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

class CSoundSystemManager:virtual public CSystemObjectBase, virtual public ISoundManager
{
	unsigned int m_nMasterVolume;// 0,100
	bool m_bEnable3DSound;

	ALCcontext *m_pContext;
	ALCdevice *m_pDevice;

	CVector m_vListenerPosition;
	CVector m_vListenerOrientation;
	CVector m_vListenerVelocity;
	
	void UpdateListener();
	
public:
	
	// ISoundManager
	
	void SetListenerPosition(CVector vPosition);
	void SetListenerOrientation(CVector vOrientation);
	void SetListenerVelocity(CVector vVelocity);

	bool Is3DSoundEnabled();

	unsigned int  GetMasterVolume();
	void SetMasterVolume(unsigned int dVolume);

	BEGIN_PROP_MAP(CSoundSystemManager)
		PROP_VALUE_FLAGS(m_nMasterVolume,"Volume",100,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_bEnable3DSound,"3DSound",false,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

	bool Unserialize(ISystemPersistencyNode *piNode);
	
	CSoundSystemManager();
	virtual ~CSoundSystemManager();
};
