#pragma once

DECLARE_CUSTOM_WRAPPER1(CEntityTypeWrapper,IEntityType,m_piEntityType)

class CEntityAnimationObjectType: public CAnimationObjectTypeBase, 
public virtual IEntityAnimationObjectTypeDesign,
public virtual IAnimationObjectTypePositionDesign,
public virtual IAnimationObjectTypeOrientationDesign
{
public:
	
	CEntityTypeWrapper  		m_EntityType;
	CVector              		m_vPosition;
	CVector              		m_vAngles;
	unsigned int                m_nTime;
	
	IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);
	std::string 	  GetAnimationObjectDescription();
	
	BEGIN_PROP_MAP(CEntityAnimationObjectType)
	PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
	PROP(m_EntityType,"Entity")
	PROP_FLAGS(m_vPosition,"Position",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_FLAGS(m_vAngles,"Angles",MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nTime,"Time",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP();
	
	void DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles ,bool bSelected);
	
	// IEntityAnimationObjectTypeDesignn
	
	void GetConfig(SEntityAnimationObjectTypeConfig *pConfig);
	void SetConfig(SEntityAnimationObjectTypeConfig *pConfig);
	void SetEntityType(IEntityType *piEntityType);
	void GetEntityType(IEntityType **ppiEntityType);
	
	// IAnimationObjectTypePositionDesign
	
	CVector GetPosition();
	void	SetPosition(CVector vPosition);
	
	// IAnimationObjectTypeOrientationDesign
	
	CVector GetAngles();
	void	SetAngles(CVector vAngles);
	
	CEntityAnimationObjectType();
	~CEntityAnimationObjectType();
};


class CEntityAnimationObject: public CAnimationObjectBase
{
	CEntityAnimationObjectType  *m_pType;
	bool						 m_bEntityCreated;
	
	//void UpdatePositionAndAngles();
	
public:
	
	void Activate(unsigned int dwCurrentTime);
	void Deactivate();
	
	bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
	void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);
	
	CEntityAnimationObject(CEntityAnimationObjectType *pType,IAnimation *piAnimation);
	~CEntityAnimationObject();
};
