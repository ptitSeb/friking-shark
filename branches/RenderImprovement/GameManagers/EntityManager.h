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

class CEntityManager: virtual public CSystemObjectBase, virtual public IEntityManager, virtual public IGameManager
{
    vector<IEntity *>	 m_lNewEntities;
    list<IEntity *>	     m_lEntities;

    void RemoveEntity(IEntity *pEntity); 

public:

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

    // IEntityManager
    void AddEntity(IEntity *pEntity); 
    void RemoveAllEntities();
	unsigned long GetEntityCount();

	void RenderEntities(IGenericRender *piRender,IGenericCamera *piCamera);

    void PerformUnaryOperation(EntityUnaryOperation pFunc,void *pParam1,void *pParam2);
    void PerformBinaryOperation(EntityBinaryOperation pFunc,void *pParam1,void *pParam2);

    IEntity *FindEntity(string sName);

    CEntityManager();
    ~CEntityManager();
};