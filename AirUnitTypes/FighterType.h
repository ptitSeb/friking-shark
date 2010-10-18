#pragma once

class CFighterType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,DWORD dwCurrentTime);

    double  m_dMaxSpeed;
    double  m_dMaxHealth;
    double  m_dMaxRoll;
    double  m_dMaxAngularSpeed;
    double  m_dTimeFirstShotMin;
    double  m_dTimeFirstShotMax;
    double  m_dTimeBetweenShotsMin;
    double  m_dTimeBetweenShotsMax;

    BEGIN_PROP_MAP(CFighterType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
        PROP_VALUE_FLAGS(m_dMaxSpeed,"Velocidad",40,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxHealth,"Vida",1,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxRoll,"GiroBarrilMaximo",40,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"VelocidadAngularMaxima",60,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dTimeFirstShotMin,"TiempoPrimerDisparoMinimo",500,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dTimeFirstShotMax,"TiempoPrimerDisparoMaximo",5000,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dTimeBetweenShotsMin,"TiempoEntreDisparosMinimo",2000,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dTimeBetweenShotsMax,"TiempoEntreDisparosMaximo",5000,MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();

    CFighterType();
    ~CFighterType();
};


class CFighter: public CEntityBase
{
    CFighterType  *m_pType;

    int m_nRoutePoint;

    double m_dExitYaw;
    double m_dExitPitch;
    double m_dwNextShotTime;

public:

    void Render(IGenericRender *piRender,IGenericCamera *piCamera);
    void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction);

    void OnKilled();
    bool OnCollision(IEntity *pOther,CVector &vCollisionPos);
    IEntity *GetTarget();

    CFighter(CFighterType *pType,DWORD dwCurrentTime);
};