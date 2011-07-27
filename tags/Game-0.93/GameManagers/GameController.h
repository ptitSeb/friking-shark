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

class CGameController:virtual public CSystemObjectBase, virtual public IGameController
{
    map<unsigned int,IGameManager *> m_mManagers;

	ISystem *m_piResourcesSystem;
	ISystem *m_piParticleSystemTypesSystem;
	ISystem *m_piAnimationTypesSystem;
	ISystem *m_piEntityTypesSystem;
	ISystem *m_piFormationTypesSystem;
	ISystem *m_piWeaponTypesSystem;

public:

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    // IGameController

	bool SetupGame();
	void EndGame();

	void CreateScenario();
	bool LoadScenario(std::string sScenario);
	bool SaveScenario(std::string sScenario);
	void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    virtual bool RegisterManager(unsigned int dwHeight,IGameManager *piManager);
    virtual void UnregisterManager(IGameManager *piManager);

    CGameController();
    virtual ~CGameController();
};
