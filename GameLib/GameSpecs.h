#ifndef _GAME_SPECS_INCLUDED_
#define _GAME_SPECS_INCLUDED_

#define PHYSIC_METER						1.0
#define PHYSIC_SECOND						1000.0

#define PHYSIC_FORCE_NORMAL					1 // es una fuerza normal, que se aplica y genera una velocidad que depende de la masa del objeto
#define PHYSIC_FORCE_CONSTANT_ACCELERATION	2 // es un fuerza que es proporcional a la masa del objeto (la gravedad), genera una aceleracion constante.

#define PHYSIC_MOVE_TYPE_NONE				0 // Para entidades que no se mueven
#define PHYSIC_MOVE_TYPE_NORMAL			1
#define PHYSIC_MOVE_TYPE_FLY				2
#define PHYSIC_MOVE_TYPE_CUSTOM			3

#define PHYSIC_BOUNDS_TYPE_NONE				0
#define PHYSIC_BOUNDS_TYPE_BBOX				1
#define PHYSIC_BOUNDS_TYPE_BSP				2

#define PHYSIC_COLLISION_TYPE_STUCK			1
#define PHYSIC_COLLISION_TYPE_SLIDE			2
#define PHYSIC_COLLISION_TYPE_BOUNCE		3
#define PHYSIC_COLLISION_TYPE_THROUGH		4 // el objeto no modifica su posicion en la colision "pasa a traves"

#define RENDER_ENABLE_AUTO_NORMALS			0x0001
#define RENDER_ENABLE_LIGHTING				0x0002
#define RENDER_ENABLE_FOG					0x0004
#define RENDER_ENABLE_TEXTURES				0x0008
#define RENDER_SHOW_LIGHTS					0x0010
#define RENDER_SHOW_BBOXES					0x0020
#define RENDER_ENABLE_MODEL_LOD				0x0040
#define RENDER_ENABLE_TRANSPARENCY			0x0080
#define RENDER_ENABLE_WORLD_LIGHTING        0x0100
#define RENDER_ENABLE_SOLID					0x0200
#define RENDER_ENABLE_MIPMAPS				0x0400
#define RENDER_SHOW_NORMALS					0x0800

#define ENTITY_ALIGNMENT_NEUTRAL		  0x0000
#define ENTITY_ALIGNMENT_PLAYER 		  0x0001
#define ENTITY_ALIGNMENT_ENEMIES      0x0002

#define DAMAGE_TYPE_NONE    0
#define DAMAGE_TYPE_NORMAL  1

#define	MAX_DETAIL_LEVELS		            10

class IGenericModel;
class IGenericCamera;
class IGenericTexture;
class IGenericRender;
class IGameGUIManager;

struct IEntity;
struct IEntityType;
struct IFormationType;
struct IPlayAreaElement;


struct SPhysicForce
{
    DWORD   dwForceType;
    CVector vDir;
    double  dConstantAccel;
    double  dForce;
    double	dMaxVelocity;

    SPhysicForce()
    {
        dwForceType=PHYSIC_FORCE_NORMAL;
        dConstantAccel=0.0;
        dForce=0;
        dMaxVelocity=0;
    }
}; 

struct SPhysicInfo
{
    DWORD					dwMoveType;
    DWORD					dwBoundsType;
    DWORD					dwCollisionType;
    double					dBounceFactor;
    double					dSlideFactor;

    CVector					vPosition;
    CVector					vVelocity;
    double					dMass;
    CVector					vAngles;
    CVector					vAngleVelocity;
    CVector					vMins;
    CVector					vMaxs;
    vector<SPhysicForce>	vForces;
    SPhysicForce			fOwnForce;

    SPhysicInfo()
    {
        dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
        dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
        dwCollisionType=PHYSIC_COLLISION_TYPE_STUCK;
        dMass=1.0;
        dBounceFactor=1.0;
        dSlideFactor=1.0;
    }
};


struct SModelDetailsInfo
{
    double  dMinDistance;
    CModel *pModel;

    SModelDetailsInfo()
    {
        dMinDistance=0;
        pModel=NULL;
    }
};

class CDetailLevelModel
{
public:
    int					 m_nDetailLevels;
    SModelDetailsInfo	 m_DetailLevels[MAX_DETAIL_LEVELS];

    CDetailLevelModel(){m_nDetailLevels=0;memset(&m_DetailLevels,0,sizeof(m_DetailLevels));}
};

struct IPhysicManager:virtual public ISystemUnknown
{
public:

    virtual CVector ProcessPhysicInfo(SPhysicInfo *pInfo,double dInterval)=0;
};

struct IDynamicLight
{
    virtual CVector GetColor()=0;
    virtual CVector GetPosition()=0;
    virtual double  GetRadius()=0;

    virtual void SetColor(CVector cColor)=0;
    virtual void SetPosition(CVector vPosition)=0;
    virtual void SetRadius(double dRadius)=0;

    virtual void SetSphericalLight(CVector cColor,CVector vPosition,double dRadius)=0;

    ~IDynamicLight(){}
};
struct IGameRender:virtual public ISystemUnknown
{
    virtual DWORD	EnableFlags(DWORD dwFlag)=0;
    virtual DWORD	DisableFlags(DWORD dwFlag)=0;
    virtual DWORD	GetFlag(DWORD dwFlag)=0;
    virtual DWORD	ToggleFlag(DWORD dwFlag)=0;

	virtual void    Render(IGenericRender *piRender,IGenericCamera *piCamera)=0;
/*
    virtual int		AddLight(CLight *pLight)=0;

	virtual void	RenderBBox(CVector vMins,CVector vMaxs)=0;
    virtual void	RenderTextureRect(CTexture *pTexture,CVector vCenter,CVector vAxis1,CVector vAxis2,double s1,double s2)=0;
    virtual void	RenderTextureRect(int nTextureIndex,CVector vCenter,CVector vAxis1,CVector vAxis2,double s1,double s2)=0;
 

	virtual void    RenderModel(IGenericModel *piModel,unsigned long nAnimation,unsigned long nFrame,bool bApplyModelMaterials)=0;

    virtual IDynamicLight  *AllocDynamicLight()=0;
    virtual void			FreeDynamicLight(IDynamicLight *pLight)=0;

    virtual void	PrepareOpenGLLighting(IEntity *piEntity)=0;
    virtual void	UnPrepareOpenGLLighting(IEntity *piEntity)=0;
*/
//	virtual void     SetCamera(IGenericCamera *piCamera)=0;
//	virtual IGenericCamera *GetCamera()=0;

	//virtual void	ResetLights()=0;
};


struct IRoute
{
    virtual unsigned    GetPointCount()=0;
    virtual CVector     GetPoint(unsigned nIndex)=0;
    virtual CVector     GetDirection(unsigned nSection)=0;
    virtual unsigned    GetNextPointIndex(unsigned nIndex)=0;
    virtual void        ProcessPoint(IEntity *piEntity,DWORD dwCurrentFrame,double dTimeFraction)=0;
};

struct IEntity:virtual public IPublisher
{
public:
    virtual string      *GetEntityClass()=0;
    virtual string      *GetEntityName()=0;
    virtual SPhysicInfo *GetPhysicInfo()=0;

    virtual DWORD GetAlignment()=0;
    virtual void  SetAlignment(DWORD dwAlignment)=0;

    virtual double GetHealth()=0;
    virtual double GetMaxHealth()=0;

    virtual bool  IsRemoved()=0;
    virtual void  Remove()=0;

    virtual void  ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)=0;
	  virtual DWORD GetNextProcessFrame()=0;

    virtual bool OnCollision(IEntity *piOther,CVector &vCollisionPos)=0;
    virtual void OnDamage(double dDamage,IEntity *pAggresor)=0;
    virtual void OnAnimationEvent(string sEvent,string sParams)=0;
    virtual void ProcessAnimations(DWORD dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished)=0;

    virtual void Render(CMaterial *pMaterial)=0;
    virtual void Render(IGenericRender *piRender,IGenericCamera *piCamera)=0;

	virtual CTraceInfo GetTrace(const CVector &p1,const CVector &p2)=0;

    virtual void SetRoute(IRoute *piRoute)=0;
    virtual IEntity *GetTarget()=0;
		virtual DWORD GetDamageType()=0;

    virtual ~IEntity(){}
};

struct IEntityEvents
{
public:
    virtual void OnRemoved(IEntity *piEntity){};
    virtual void OnKilled(IEntity *piEntity){};
};

typedef void (*EntityUnaryOperation)(IEntity *piEntity,void *pParam1,void *pParam2);
typedef void (*EntityBinaryOperation)(IEntity *piEntity1,IEntity *piEntity2,void *pParam1,void *pParam2);

struct IEntityManager:virtual public ISystemUnknown
{
public:
	

	virtual void AddEntity(IEntity *pEntity)=0; 
	virtual void RemoveAllEntities()=0;
	virtual unsigned long GetEntityCount()=0;

    virtual void PerformUnaryOperation(EntityUnaryOperation pFunc,void *pParam1,void *pParam2)=0;
    virtual void PerformBinaryOperation(EntityBinaryOperation pFunc,void *pParam1,void *pParam2)=0;

    virtual IEntity *FindEntity(string sName)=0;
};

struct IGameManager:virtual public ISystemUnknown
{
public:

	virtual void CreateScenario()=0;
    virtual void LoadScenario(ISystemPersistencyNode *piNode)=0;
	virtual void SaveScenario(ISystemPersistencyNode *piNode)=0;
    virtual void CloseScenario()=0;

    virtual void Start()=0;
    virtual void Stop()=0;
    virtual void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)=0;
};

struct STerrainWater
{
	bool		 bEnabled;
	double		 dSpeed;
	double		 dOpacity;
	double		 dHorizontalResolution;
	double		 dVerticalResolution;
	std::string  sTextureFile1;
	std::string  sTextureFile2;
	double		 dHeight;

	STerrainWater()
	{
		dOpacity=1;
		dSpeed=0;
		bEnabled=false;
		dHorizontalResolution=1;
		dVerticalResolution=1;
		dHeight=0;
	}
};

struct STerrainSky
{
	bool		 bEnabled;
	double		 dSpeed;
	double		 dOpacity;
	double		 dHorizontalResolution;
	double		 dVerticalResolution;
	std::string  sTextureFile;

	STerrainSky()
	{
		dOpacity=1;
		dSpeed=0;
		bEnabled=false;
		dHorizontalResolution=1;
		dVerticalResolution=1;
	}
};

struct STerrainSun
{
	double		 dDistance;
	double		 dElevation;
	double		 dAzimuth;
	CVector		 vColor;

	STerrainSun()
	{
		dDistance=1;
		dElevation=90;
		dAzimuth=0;
		vColor=CVector(0.2,0.2,0.2);
	}
};

struct STerrainFog
{
	bool		 bEnabled;
	double		 dStart;
	double		 dEnd;
	CVector		 vColor;

	STerrainFog()
	{
		bEnabled=false;
		dStart=100;
		dEnd=1000;
		vColor=CVector(1,1,1);
	}
};

struct STerrainHeightLayer
{
	double dMinHeight;
	double dMaxHeight;
	double dHorizontalResolution;
	double dVerticalResolution;
	double dDecayMargin;

	std::string sTextureFile;

	STerrainHeightLayer()
	{
		dMinHeight=0;
		dMaxHeight=0;
		dHorizontalResolution=1;
		dVerticalResolution=1;
		dDecayMargin=0;
	}
};

struct STerrainColorLayer
{
	CVector vColor;
	double dHorizontalResolution;
	double dVerticalResolution;

	std::string sTextureFile;

	// Componentes Saturacion y Valor en HSV

	double dColorSaturationMargin;
	double dColorValueMargin;
	bool   bAbruptEnd;

	STerrainColorLayer()
	{
		dColorSaturationMargin=1.0;
		dColorValueMargin=0;
		dHorizontalResolution=1;
		dVerticalResolution=1;
		bAbruptEnd=false;
	}
};

struct SEntityLayer
{
	double dSeparation;
	double dPositionJitter;
	double dYawJitter;

	CVector vColor;
	double dColorSaturationMargin;
	double dColorValueMargin;

	std::string	sEntityType;

	SEntityLayer()
	{
		dSeparation=1;
		dPositionJitter=0;
		dYawJitter=0;
		dColorSaturationMargin=1;
		dColorValueMargin=0;
	}
};

struct IDesign:virtual public ISystemUnknown
{
public:
	// IDesign General settings

	virtual void DesignRender(IGenericRender *piRender)=0;
};

struct IDesignObject:virtual public ISystemUnknown
{
public:
	// IDesign General settings

	virtual void DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected)=0;
	virtual void DesignGetBBox(CVector *pvMins,CVector *pvMaxs)=0;
	virtual CTraceInfo DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )=0;
	virtual double DesignGetRadius()=0;
};

struct IEntityType:virtual public ISystemUnknown,virtual public IDesignObject
{
	virtual IEntity *CreateInstance(IEntity *piParent,DWORD dwCurrentTime)=0;

	virtual void	GetBBox(CVector *pvMins,CVector *pvMaxs)=0;
	virtual CVector	GetSize()=0;

};

struct IFormationEvents:virtual public ISystemUnknown
{
	virtual void OnFormationRemoved(ISystemObject *piFormation)=0;
	virtual void OnFormationKilled(ISystemObject *piFormation,IEntity *piLastEntity)=0;
};

struct IFormation:virtual public ISystemUnknown
{
	virtual bool ProcessFrame(DWORD dwCurrentTime,double dInterval)=0;
};

struct IFormationType:virtual public ISystemUnknown,virtual public IDesignObject
{
	virtual IFormation *CreateInstance(CVector vPosition,DWORD dwCurrentTime)=0;
};


struct IPlayAreaCheckPoint:virtual public ISystemUnknown
{
	virtual CVector GetCheckPointPosition()=0;
};

struct SPlayAreaConfig
{
	double dCameraScroll;
	double dCameraDistance;
	double dCameraAspectRatio;
	double dCameraSpeed;
	double dCameraViewAngle;
	double dAirPlaneHeight;

	SPlayAreaConfig(){dCameraScroll=0;dCameraDistance=0;dCameraAspectRatio=0;dCameraSpeed=0;dCameraViewAngle=0;}
};

struct IPlayAreaDesign:virtual public IDesign
{
public:

	virtual void	GetPlayAreaConfig(SPlayAreaConfig *pConfig)=0;
	virtual void	SetPlayAreaConfig(SPlayAreaConfig *pConfig)=0;
	virtual void	UpdatePlayArea()=0;

	// Element management

	virtual unsigned long	AddElement(std::string sType)=0;
	virtual void			RemoveElement(unsigned int nIndex)=0;
	virtual void			GetElement(unsigned int nIndex,IPlayAreaElement **ppiElement)=0;
	virtual unsigned int	GetElements()=0;

	// IPlayAreaDesign Color based Entity layers

	virtual unsigned long	AddEntityLayer(SEntityLayer *pLayer)=0;
	virtual void			UpdateEntityLayer(unsigned int nIndex,SEntityLayer *pLayer)=0;
	virtual void			RemoveEntityLayer(unsigned int nIndex)=0;
	virtual void			GetEntityLayer(unsigned int nIndex,SEntityLayer *pLayer,IEntityType **ppiEntityType)=0;
	virtual unsigned int	GetEntityLayers()=0;

	virtual bool			UpdateEntityLayers()=0;
};

struct ITerrainDesign:virtual public IDesign
{
public:
	// ITerrainDesign General settings

	virtual bool SetTerrainBaseModel(std::string sModel)=0;
	virtual bool SetTerrainColorMap(std::string sColorMap)=0;

	virtual void GetTerrainBaseModel(std::string *psModel,IGenericModel **ppiModel)=0;
	virtual void GetTerrainColorMap(std::string *psColorMap,IGenericTexture **ppiTexture)=0;

	virtual void GetTerrainModel(IGenericModel **ppiModel)=0;

	// Water

	virtual bool SetTerrainWater(STerrainWater *pWater)=0;
	virtual void GetTerrainWater(STerrainWater *pWater,IGenericTexture **ppiTexture1,IGenericTexture **ppiTexture2)=0;

	// Sky

	virtual bool SetTerrainSky(STerrainSky *pSky)=0;
	virtual void GetTerrainSky(STerrainSky *pSky,IGenericTexture **ppiTexture)=0;

	// Sun

	virtual bool SetTerrainSun(STerrainSun *pSun)=0;
	virtual void GetTerrainSun(STerrainSun *pSun)=0;

	// Ambient Color

	virtual void	SetTerrainAmbientColor(const CVector &vAmbientColor)=0;
	virtual CVector GetTerrainAmbientColor()=0;

	// Fog

	virtual bool SetTerrainFog(STerrainFog *pFog)=0;
	virtual void GetTerrainFog(STerrainFog *pFog)=0;

	// ITerrainDesign Color layers

	virtual unsigned long	AddTerrainColorLayer(STerrainColorLayer *pLayer)=0;
	virtual void			UpdateTerrainColorLayer(unsigned int nIndex,STerrainColorLayer *pLayer)=0;
	virtual unsigned int	MoveTerrainColorLayer(unsigned int nIndex,bool bUp)=0;
	virtual void			RemoveTerrainColorLayer(unsigned int nIndex)=0;
	virtual void			GetTerrainColorLayer(unsigned int nIndex,STerrainColorLayer *pLayer,IGenericTexture **ppiTexture)=0;
	virtual unsigned int	GetTerrainColorLayers()=0;

	// ITerrainDesign Height layers

	virtual unsigned long	AddTerrainHeightLayer(STerrainHeightLayer *pLayer)=0;
	virtual void			UpdateTerrainHeightLayer(unsigned int nIndex,STerrainHeightLayer *pLayer)=0;
	virtual unsigned int	MoveTerrainHeightLayer(unsigned int nIndex,bool bUp)=0;
	virtual void			RemoveTerrainHeightLayer(unsigned int nIndex)=0;
	virtual void			GetTerrainHeightLayer(unsigned int nIndex,STerrainHeightLayer *pLayer,IGenericTexture **ppiTexture)=0;
	virtual unsigned int	GetTerrainHeightLayers()=0;

	virtual bool			UpdateTerrain()=0;

	// Information methods
	virtual bool			GetTerrainStats( double *pdVertexOverhead,double *pdFaceOverhead )=0;
	virtual bool			GetTerrainHeightAt( CVector vPoint ,double *pdHeight)=0;
	virtual bool			GetTerrainTrace( CVector vPoint1 ,CVector vPoint2 ,CVector *pHitPos)=0;
};

struct IWorldManager:virtual public ISystemUnknown
{
public:
	virtual void SetupRenderingEnvironment(IGenericRender *piRender)=0;
};

struct IFrameManager:virtual public ISystemUnknown
{
public:
		virtual void Reset()=0;
		virtual bool IsPaused()=0;
		virtual void TogglePauseOnNextFrame()=0;
		virtual void SetPauseOnNextFrame(bool bPause)=0;

		virtual void ProcessFrame()=0;

		virtual DWORD  GetCurrentRealTime()=0;
		virtual DWORD  GetCurrentTime()=0;
		virtual double GetTimeFraction()=0;
		virtual double GetRealTimeFraction()=0;
		virtual double GetCurrentFps()=0;
};

struct IGameController:virtual public ISystemUnknown
{
	virtual bool SetupGame()=0;
	virtual void EndGame()=0;

	virtual void CreateScenario()=0;
	virtual bool LoadScenario(std::string sScenario)=0;
	virtual bool SaveScenario(std::string sScenario)=0;
	virtual void CloseScenario()=0;

    virtual void Start()=0;
    virtual void Stop()=0;
    virtual void ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)=0;

    virtual bool RegisterManager(DWORD dwHeight,IGameManager *piManager)=0;
    virtual void UnregisterManager(IGameManager *piManager)=0;
};

enum GameActivationType
{
    AT_None,
    AT_EnterInPlayArea,
    AT_PlayerPosition
};

enum GameDeactivationType
{
    DT_None,
    DT_ExitFromPlayArea,
    DT_PlayerPosition,
    DT_TimeInterval
};

struct SPlayAreaInfo
{
    enum EPlayAreaPlane {Far,Near,Left,Right,Top,Bottom,PlaneCount};

    CPlane  planes[6];
    int     nPlaneCount;
    CVector vMins;
    CVector vMaxs;

    SPlayAreaInfo(){nPlaneCount=0;}
};


#define KEY_FORWARD		1
#define KEY_BACK		2
#define KEY_RIGHT		3
#define KEY_LEFT		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_PAUSE		7
#define KEY_PROCESS_ONE_FRAME		8

struct IPlayAreaElement:virtual public ISystemUnknown
{
    virtual void Activate(DWORD dwCurrentTime)=0;
    virtual void Deactivate()=0;
    virtual bool IsActive()=0;
	virtual void Reset()=0;

    virtual bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pPlayAreaInfo,DWORD dwCurrentTime,double dInterval)=0;

	virtual void DesignRender(IGenericRender *piRender,bool bSelected)=0;
	virtual CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2)=0;
};

struct IPlayAreaEntity:virtual public IPlayAreaElement
{
	virtual void SetPosition(const CVector &vPosition)=0;
	virtual void SetAngles(const CVector &vAngles)=0;
	virtual void SetEntityType(IEntityType *piEntityType)=0;

	virtual CVector GetPosition()=0;
	virtual CVector GetAngles()=0;
	virtual void	GetEntityType(IEntityType **piEntityType)=0;
};
	

struct IPlayAreaFormation:virtual public IPlayAreaElement
{
	virtual void SetPosition(CVector &vPosition)=0;
	virtual void SetFormationType(IFormationType *piFormationType)=0;
	virtual void SetBonusType(IEntityType *piEntityType)=0;

	virtual CVector GetPosition()=0;
	virtual void	GetFormationType(IFormationType **piFormationType)=0;
	virtual void	GetBonusType(IEntityType **ppiEntityType)=0;
};


class IPlayAreaElementEnumerationCallback
{
public:
	virtual void ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating)=0;

	virtual ~IPlayAreaElementEnumerationCallback(){}
};

struct IPlayAreaManager:virtual public ISystemUnknown
{
    virtual void GetAirPlayPlane(CVector *pPlayAreaMins,CVector *pPlayAreaMaxs)=0;
    virtual void GetVisibleAirPlayPlane(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs)=0;
	virtual void GetPlayAreaPlaneAt(CVector vPos,CVector *pPlayAreaMins,CVector *pPlayAreaMaxs)=0;

	virtual void	SetPlayMovementPosition(CVector vPosition)=0;
    virtual CVector GetPlayMovementPosition()=0;
    virtual CVector GetPlayMovementForward()=0;
    virtual CVector GetPlayMovementRight()=0;
    virtual CVector GetPlayMovementUp()=0;
	virtual void	GetPlayerRoute(CVector *pvStart,CVector *pvEnd)=0;

	virtual IGenericCamera *GetCamera()=0;

	virtual void EnumeratePlayAreaElements(IPlayAreaElementEnumerationCallback *piCallback)=0;

	virtual void ProcessInput(IGameGUIManager *piManager)=0;
	virtual bool IsScenarioCompleted()=0;
};

struct IWeapon
{
  virtual DWORD GetSlot()=0;

  virtual void  Fire(DWORD dwCurrentTime)=0;
  virtual bool  IsReady(DWORD dwCurrentTime)=0;

  virtual DWORD GetCurrentLevel()=0;
  virtual void  SetCurrentLevel(DWORD dwLevel)=0;

  virtual ~IWeapon(){}
};

struct IWeaponType:virtual public ISystemUnknown
{
  virtual IWeapon *CreateInstance(IEntity *piEntity,DWORD dwCurrentTime)=0;
};


struct IPlayer
{
  virtual DWORD GetPoints()=0;
  virtual void  SetPoints(DWORD dwPoints)=0;
  virtual void  AddPoints(DWORD dwPoints)=0;

  virtual DWORD GetLivesLeft()=0;
  virtual void  SetLivesLeft(DWORD dwLivesLeft)=0;
  virtual void  AddLivesLeft(DWORD dwLivesLeft)=0;

  virtual double GetSpeed()=0;
  virtual void   SetSpeed(double dSpeed)=0;

  virtual void  GetWeapons(vector<IWeapon*> *pWeapons)=0;
  virtual void  GetWeaponsOnSlot(DWORD dwWeaponSlot,vector<IWeapon*> *pWeapons)=0;
  virtual void  FireWeaponsOnSlot(DWORD dwWeaponSlot,DWORD dwCurrentTime)=0;

  virtual ~IPlayer(){}
};
#endif