#pragma once

class CLightParticleType: virtual public CSystemObjectBase,virtual public IParticleType
{
public:
	CGenericRenderWrapper m_Render;


    IParticle *CreateInstance(IParticleEmitter *piEmiter,DWORD dwCurrentTime);

	bool Init(std::string sClass,std::string sName,ISystem *piSystem);
	void Destroy();

    CLightParticleType();
    ~CLightParticleType();
};

class CLightParticle: public IParticle
{
    CLightParticleType  *m_pType;
    CGenericLightWrapper m_Light;

public:

    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CLightParticle(CLightParticleType *pType,IParticleEmitter *pEmiter);
    ~CLightParticle();
};
