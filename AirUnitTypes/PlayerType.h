#pragma once

class CPlayerType: public CEntityTypeBase
{
    double m_dMaxSpeed;
public:
    IEntity *CreateInstance(IEntity *piParent,DWORD dwCurrentTime);

    BEGIN_PROP_MAP(CPlayerType)
      PROP_VALUE_FLAGS(m_dMaxSpeed,"Velocidad",80,MRPF_NORMAL|MRPF_OPTIONAL);
      PROP_CLASS_CHAIN(CEntityTypeBase)
    END_PROP_MAP();

    CPlayerType();
    ~CPlayerType();
};


class CPlayer: public CEntityBase,public IPlayer
{
    DWORD m_dwPoints;
    DWORD m_dwLivesLeft;
    double m_dSpeed;

    CPlayerType  *m_pType;

		bool OnCollision(IEntity *piOther,CVector &vCollisionPos);
		void OnKilled();

public:
	//void Render(IGenericRender *piRender,IGenericCamera *piCamera){}
    DWORD GetPoints();
    void  SetPoints(DWORD dwPoints);
    void  AddPoints(DWORD dwPoints);

    DWORD GetLivesLeft();
    void  SetLivesLeft(DWORD dwLivesLeft);
    void  AddLivesLeft(DWORD dwLivesLeft);

    double GetSpeed();
    void   SetSpeed(double dSpeed);

    void  GetWeapons(vector<IWeapon*> *pWeapons);
    void  GetWeaponsOnSlot(DWORD dwWeaponSlot,vector<IWeapon*> *pWeapons);
    void  FireWeaponsOnSlot(DWORD dwWeaponSlot,DWORD dwCurrentTime);

    CPlayer(CPlayerType *pType);
};