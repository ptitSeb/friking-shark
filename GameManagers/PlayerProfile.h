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

#define DEFAULT_JOYSTICK_DEAD_ZONE 0.25

BEGIN_STRUCT_PROPS(SKeyCombination)
	PROP(nKey,"Key");
	PROP(nModifierA,"ModifierA");
	PROP(nModifierB,"ModifierB");
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SKeyMapping)
	PROP(vValidCombinations,"ValidCombinations");
	PROP(sFriendlyName,"FriendlyName");
END_STRUCT_PROPS()

BEGIN_STRUCT_PROPS(SJoystickButtonMapping)
	PROP(nButton,"Button");
	PROP(sFriendlyName,"FriendlyName");
END_STRUCT_PROPS()

class CPlayerProfile: virtual public CSystemObjectBase,
                        virtual public CSystemSerializableBase,
                        virtual public IPlayerProfile
{
	
	std::map<std::string,SKeyMapping> m_KeyboardMapping;
	std::map<std::string,SJoystickButtonMapping> m_JoystickMapping;
	double		m_dDifficulty;
	std::string m_sPlayerName;
	double      m_dJoystickDeadZone;
	std::string m_sJoystickDevice;

public:

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();
	
	void 		SetPlayerName(std::string sName);
	std::string GetPlayerName();
	
	void 	SetDifficulty(double dDifficulty);
	double 	GetDifficulty();
	
	void 	GetKeyboardMapping(std::map<std::string,SKeyMapping> *pMapping);
	void 	SetKeyboardMapping(std::map<std::string,SKeyMapping> *pMapping);
	
	void 	GetKeyMapping(std::string,SKeyMapping *pMapping);
	void 	SetKeyMapping(std::string,SKeyMapping *pMapping);
	
	double  GetJoystickDeadZone();
	void    SetJoystickDeadZone(double dZone);
	
	void 	GetJoystickMapping(std::map<std::string,SJoystickButtonMapping> *pMapping);
	void 	SetJoystickMapping(std::map<std::string,SJoystickButtonMapping> *pMapping);
	
	void 	GetJoystickButtonMapping(std::string,SJoystickButtonMapping *pMapping);
	void 	SetJoystickButtonMapping(std::string,SJoystickButtonMapping *pMapping);
	
	void 	    SetJoystickDevice(const std::string &sDevice);
	std::string GetJoystickDevice();

	BEGIN_PROP_MAP(CPlayerProfile)
		PROP(m_sPlayerName,"Name");
		PROP(m_dDifficulty,"Difficulty");
		PROP(m_KeyboardMapping,"KeyboardMapping");
		PROP_FLAGS(m_JoystickMapping,"JoystickMapping",MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dJoystickDeadZone,"JoystickDeadZone",DEFAULT_JOYSTICK_DEAD_ZONE,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_FLAGS(m_sJoystickDevice,"JoystickDevice",MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();

    CPlayerProfile(void);
    ~CPlayerProfile(void);
};
