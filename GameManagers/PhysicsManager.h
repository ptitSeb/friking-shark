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


class CPhysicManager:virtual public CSystemObjectBase, virtual public IPhysicManager, virtual public IGameManager
{
    vector<SPhysicForce> m_vGlobalForces;
    void ApplyForce(SPhysicInfo *pInfo,SPhysicForce *pForce,double dInterval);

public:

    CEntityManagerWrapper   m_EntityManagerWrapper;
    CGameControllerWrapper  m_GameControllerWrapper;

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    // IGameManager
	void CreateScenario();
	void LoadScenario(ISystemPersistencyNode *piNode);
    void SaveScenario(ISystemPersistencyNode *piNode);
    void CloseScenario();

    void Start();
    void Stop();
    void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    // IPhysicManager
    CVector ProcessPhysicInfo(SPhysicInfo *pInfo,double dInterval);

    CPhysicManager();
    virtual ~CPhysicManager();
};
