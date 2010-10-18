#pragma once

class CBomberType: public CEntityTypeBase
{
public:
  double  m_dMaxHealth;
  double  m_dMaxVelocity;
  double  m_dTimeFirstShotMin;
  double  m_dTimeFirstShotMax;
  double  m_dTimeBetweenShotsMin;
  double  m_dTimeBetweenShotsMax;

  IEntity *CreateInstance(IEntity *piParent,DWORD dwCurrentTime);

  BEGIN_PROP_MAP(CBomberType)
    PROP_VALUE_FLAGS(m_dMaxHealth,"Vida",20,MRPF_NORMAL|MRPF_OPTIONAL);
    PROP_VALUE_FLAGS(m_dMaxVelocity,"Velocidad",20,MRPF_NORMAL|MRPF_OPTIONAL);
    PROP_VALUE_FLAGS(m_dTimeFirstShotMin,"TiempoPrimerDisparoMinimo",500,MRPF_NORMAL|MRPF_OPTIONAL);
    PROP_VALUE_FLAGS(m_dTimeFirstShotMax,"TiempoPrimerDisparoMaximo",5000,MRPF_NORMAL|MRPF_OPTIONAL);
    PROP_VALUE_FLAGS(m_dTimeBetweenShotsMin,"TiempoEntreDisparosMinimo",2000,MRPF_NORMAL|MRPF_OPTIONAL);
    PROP_VALUE_FLAGS(m_dTimeBetweenShotsMax,"TiempoEntreDisparosMaximo",5000,MRPF_NORMAL|MRPF_OPTIONAL);
    PROP_CLASS_CHAIN(CEntityTypeBase)
  END_PROP_MAP();

  CBomberType();
  ~CBomberType();
};


class CBomber: public CEntityBase
{
  unsigned  m_nRoutePoint;
  double    m_dwNextShotTime;

  CBomberType  *m_pType;

public:

  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void OnKilled();
  void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction);
  IEntity *GetTarget();

  CBomber(CBomberType *pType,DWORD dwCurrentTime);
};