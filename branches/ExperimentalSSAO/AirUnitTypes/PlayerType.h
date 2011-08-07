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

enum EPlayerState
{
	ePlayerState_Normal=ENTITY_STATE_BASE,
	ePlayerState_Falling,
	ePlayerState_Crashed
};

class CPlayerType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);
	
	double  m_dMaxAngularSpeed;
	
	BEGIN_PROP_MAP(CPlayerType)
		PROP_CLASS_CHAIN(CEntityTypeBase)
		PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"MaxAngularVelocity",20,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(ePlayerState_Falling,"Falling")
		ENTITY_STATE(ePlayerState_Crashed,"Crashed")
	END_ENTITY_STATE_MAP()
	
    CPlayerType();
    ~CPlayerType();
};


class CPlayer: public CEntityBase,public IPlayer
{
	int m_nRoutePoint;
	bool m_bRouteFinished;
	
    unsigned int m_dwPoints;
    unsigned int m_dwLivesLeft;
	unsigned int m_nCurrentTime;
	bool         m_bGodMode;
	
    double m_dSpeed;

    CPlayerType  *m_pType;

		bool OnCollision(IEntity *piOther,CVector &vCollisionPos);
		void OnKilled();
		
		void OnDamage(double dDamage,IEntity *pAggresor);
		
public:
	
	bool HasFinishedRoute();
	void SetRoute(IRoute *piRoute);
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
	
	//void Render(IGenericRender *piRender,IGenericCamera *piCamera){}
    unsigned int GetPoints();
    void  SetPoints(unsigned int dwPoints);
    void  AddPoints(unsigned int dwPoints);

    unsigned int GetLivesLeft();
    void  SetLivesLeft(unsigned int dwLivesLeft);
    void  AddLivesLeft(unsigned int dwLivesLeft);

    double GetSpeed();
    void   SetSpeed(double dSpeed);

    void  GetWeapons(vector<IWeapon*> *pWeapons);
    void  GetWeaponsOnSlot(unsigned int dwWeaponSlot,vector<IWeapon*> *pWeapons);
    void  FireWeaponsOnSlot(unsigned int dwWeaponSlot,unsigned int dwCurrentTime);
	
	void         SetGodMode(bool bGod);
	
	void Render(IGenericRender *piRender,IGenericCamera *piCamera);
	
    CPlayer(CPlayerType *pType);
};