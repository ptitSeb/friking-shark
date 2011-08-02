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


#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUI.h"
#include "PlayerProfile.h"

CPlayerProfile::CPlayerProfile(void)
{
	m_sPlayerName="Unknown";
	m_dDifficulty=0;
	m_KeyboardMapping["MoveForward"].sFriendlyName="Up";
	m_KeyboardMapping["MoveForward"].vValidCombinations.push_back(SKeyCombination(GK_UP));
	m_KeyboardMapping["MoveBackward"].sFriendlyName="Down";
	m_KeyboardMapping["MoveBackward"].vValidCombinations.push_back(SKeyCombination(GK_DOWN));
	m_KeyboardMapping["MoveLeft"].sFriendlyName="Left";
	m_KeyboardMapping["MoveLeft"].vValidCombinations.push_back(SKeyCombination(GK_LEFT));
	m_KeyboardMapping["MoveRight"].sFriendlyName="Right";
	m_KeyboardMapping["MoveRight"].vValidCombinations.push_back(SKeyCombination(GK_RIGHT));
	m_KeyboardMapping["FireBullets"].sFriendlyName="Fire";
	m_KeyboardMapping["FireBullets"].vValidCombinations.push_back(SKeyCombination(GK_LCONTROL));
	m_KeyboardMapping["FireBomb"].sFriendlyName="Bomb";
	m_KeyboardMapping["FireBomb"].vValidCombinations.push_back(SKeyCombination(GK_LMENU));
}

CPlayerProfile::~CPlayerProfile(void)
{
}

void CPlayerProfile::SetPlayerName(std::string sName)
{
	m_sPlayerName=sName;
	NOTIFY_EVENT(IPlayerProfileEvents,OnPlayerNameChanged(sName));
}

std::string CPlayerProfile::GetPlayerName(){return m_sPlayerName;}
double CPlayerProfile::GetDifficulty(){return m_dDifficulty;}

void CPlayerProfile::SetDifficulty(double dDifficulty)
{
	m_dDifficulty=dDifficulty;
	NOTIFY_EVENT(IPlayerProfileEvents,OnDifficultyChanged(m_dDifficulty));
}


void CPlayerProfile::GetKeyboardMapping(std::map<std::string,SKeyMapping> *pMapping){*pMapping=m_KeyboardMapping;}
void CPlayerProfile::SetKeyboardMapping(std::map<std::string,SKeyMapping> *pMapping)
{
	m_KeyboardMapping=*pMapping;
	NOTIFY_EVENT(IPlayerProfileEvents,OnKeyboardMappingChanged());
}

void CPlayerProfile::SetKeyMapping(std::string sName,SKeyMapping *pMapping)
{
	m_KeyboardMapping[sName]=*pMapping;
	NOTIFY_EVENT(IPlayerProfileEvents,OnKeyboardMappingChanged());
}

void CPlayerProfile::GetKeyMapping(std::string sName,SKeyMapping *pMapping)
{
	std::map<std::string,SKeyMapping>::iterator i=m_KeyboardMapping.find(sName);
	*pMapping=(i==m_KeyboardMapping.end())?SKeyMapping():i->second;
}

bool CPlayerProfile::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	return bOk;
}

void CPlayerProfile::Destroy()
{
	CSystemObjectBase::Destroy();
}
