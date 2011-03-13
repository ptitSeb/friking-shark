#pragma once

enum EPlayerState
{
	ePlayerState_Normal=ENTITY_STATE_BASE,
	ePlayerState_Falling,
	ePlayerState_Crashed
};

class CPlayerType: public CEntityTypeBase
{
    double m_dMaxSpeed;
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    BEGIN_PROP_MAP(CPlayerType)
      PROP_VALUE_FLAGS(m_dMaxSpeed,"Velocidad",80,MRPF_NORMAL|MRPF_OPTIONAL);
      PROP_CLASS_CHAIN(CEntityTypeBase)
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
    unsigned int m_dwPoints;
    unsigned int m_dwLivesLeft;
    double m_dSpeed;

    CPlayerType  *m_pType;

		bool OnCollision(IEntity *piOther,CVector &vCollisionPos);
		void OnKilled();

public:
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

	
    CPlayer(CPlayerType *pType);
};