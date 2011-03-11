#pragma once

class CTruckType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    double  m_dMaxSpeed;
    double  m_dMaxHealth;
    double  m_dMaxAngularSpeed;

    BEGIN_PROP_MAP(CTruckType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
        PROP_VALUE_FLAGS(m_dMaxSpeed,"Velocidad",40,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxHealth,"Vida",1,MRPF_NORMAL|MRPF_OPTIONAL);
        PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"VelocidadAngularMaxima",60,MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();

    CTruckType();
    ~CTruckType();
};


class CTruck: public CEntityBase
{
	IEntity *m_piTarget;
    CTruckType  *m_pType;
public:
	void SetRoute(IRoute *piRoute);
	void OnKilled();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    CTruck(CTruckType *pType);
	void Render(IGenericRender *piRender,IGenericCamera *piCamera);
};