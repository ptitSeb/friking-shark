#pragma once

class CBomberType: public CEntityTypeBase
{
public:
  double  m_dTimeFirstShotMin;
  double  m_dTimeFirstShotMax;
  double  m_dTimeBetweenShotsMin;
  double  m_dTimeBetweenShotsMax;

  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CBomberType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
	PROP_VALUE_FLAGS(m_dTimeFirstShotMin,"TimeFirstShotMin",500,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dTimeFirstShotMax,"TimeFirstShotMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dTimeBetweenShotsMin,"TimeBetweenShotsMin",2000,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dTimeBetweenShotsMax,"TimeBetweenShotsMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
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
  bool HasFinishedRoute();
  void SetRoute(IRoute *piRoute);
  
  bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
  void OnKilled();
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
  IEntity *GetTarget();

  CBomber(CBomberType *pType,unsigned int dwCurrentTime);
};